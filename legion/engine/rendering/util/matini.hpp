#pragma once
#include <istream>
#include <inih/ini.hpp>
#include <rendering/data/material.hpp>
#include <tuple>

namespace legion::rendering {

    namespace detail
    {
        class bytes_to_ini
        {
        public:
            bytes_to_ini(byte_vec data) : m_data(data) {}


            //trying to mimic how fgets work
            static char* reader(char* str, int num, void* stream)
            {
                auto& self = *static_cast<bytes_to_ini*>(stream);

                //when fgets has nothing to copy anymore it returns nullptr and leaves the memory alone
                const size_type to_read = (std::min)(self.m_data.size() - self.cursor - 1 ,size_type(num));
                if(to_read == 0)
                {
                    return nullptr;
                }

                //otherwise it copies the contents of the file into the user provided buffer
                memcpy(str,self.m_data.data() + self.cursor,to_read);
                self.cursor += to_read;
                return str;
            }

        private:
            byte_vec m_data;
            size_type cursor = 0;
        };

        class handler_to_cpp
        {
        public:
            static int handle(void* user,const char * section,const char * name, const char * value)
            {
                auto& self = *static_cast<handler_to_cpp*>(user);
                self.m_parsed.emplace(section,std::make_pair<std::string,std::string>(name,value));
                return 1;
            }

            template <class Func>
            void for_each_value_in_section(std::string section,Func&& f)
            {
                auto& [start,end] = m_parsed.equal_range(section);

                for (auto& [key,value] :  iterator::pair_range(start,end))
                {
                    std::invoke(f,key,value);
                }
            }

        private:
            std::unordered_multimap<std::string,std::pair<std::string,std::string>> m_parsed;

        };

        template <class T>
        std::pair<bool,T> convert(const std::string& str)
        {
            const std::istringstream iss(str);
            T dummy;
            iss >> dummy;
            return {iss && iss.eof(),dummy};
        }

        template <class T>
        auto convert_stream(const std::string& str,std::istringstream* iss)
        {
            T dummy;
            (*iss) >> dummy;
            return dummy;
        }


        template <class... Args>
        std::pair<bool,std::tuple<Args...>> convert_tuple(const std::string& str)
        {
            const std::istringstream iss(str);
            std::tuple<Args...> results = { convert_stream<Args>(str)... };
            return {iss && iss.eof(),results};
        }
    }



    inline void apply_material_conf(material* material,std::string section, fs::view file)
    {

        auto reader = detail::bytes_to_ini(file.get().decay().get());
        detail::handler_to_cpp handler;

        ini_parse_stream(&detail::bytes_to_ini::reader,&reader,&detail::handler_to_cpp::handle,&handler);

        handler.for_each_value_in_section(std::move(section),[material,&file](std::string key,std::string value)
        {
            if(value == "true")
                material->set_param(key,true);
            if(value == "false")
                material->set_param(key,false);

            if(value.find(".f") != std::string::npos)
            {
                if(auto [success,values] = detail::convert_tuple<float,float,float,float>(value);success)
                {
                    auto& [v1,v2,v3,v4] = values;
                    material->set_param(key,math::vec4(v1,v2,v3,v4));
                    return;
                }
                if(auto [success,values] = detail::convert_tuple<float,float,float>(value);success)
                {
                    auto& [v1,v2,v3] = values;
                    material->set_param(key,math::vec3(v1,v2,v3));
                    return;
                }
                if(auto [success,values] = detail::convert_tuple<float,float>(value);success)
                {
                    auto& [v1,v2] = values;
                    material->set_param(key,math::vec2(v1,v2));
                    return;
                }
                if(auto [success,parsed] = detail::convert<float>(value);success)
                {
                    material->set_param(value,parsed);
                    return;
                }
            }
            else
            {
                if(auto [success,values] = detail::convert_tuple<int,int,int,int>(value);success)
                {
                    auto& [v1,v2,v3,v4] = values;
                    material->set_param(key,math::ivec4(v1,v2,v3,v4));
                    return;
                }
                if(auto [success,values] = detail::convert_tuple<int,int,int>(value);success)
                {
                    auto& [v1,v2,v3] = values;
                    material->set_param(key,math::ivec3(v1,v2,v3));
                    return;
                }
                if(auto [success,values] = detail::convert_tuple<int,int>(value);success)
                {
                    auto& [v1,v2] = values;
                    material->set_param(key,math::ivec2(v1,v2));
                    return;
                }
                if(auto [success,parsed] = detail::convert<int>(value);success)
                {
                    material->set_param(value,parsed);
                    return;
                }
            } 
            material->set_param(key,rendering::TextureCache::create_texture(file.parent()/value));
            
        });

    }
}
