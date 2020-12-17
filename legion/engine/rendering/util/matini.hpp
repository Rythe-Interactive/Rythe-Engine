#pragma once

#include <inih/ini.h>
#include <rendering/data/material.hpp>
#include <tuple>
#include <sstream>
#include <vector>
namespace legion::rendering {

    namespace detail
    {
        class handler_to_cpp
        {
        public:
            //a place to store the data? maybe, not used
            static int handle(void* user, const char* section, const char* name, const char* value)
            {
                log::debug("got kv-pair: {}|{}", name, value);
                auto& self = *static_cast<handler_to_cpp*>(user);
                self.m_parsed.emplace(section, std::make_pair<std::string, std::string>(name, value));
                return 1;
            }

            //function to go through each value in a section
            template <class Func>
            void for_each_value_in_section(const std::string& section, Func&& f)
            {
                auto range = iterator::pair_range(m_parsed.equal_range(section));

                for (auto& [key, value] : iterator::values_only(range))
                {
                    std::invoke(f, key, value);
                }
            }

        private:
            std::unordered_multimap<std::string, std::pair<std::string, std::string>> m_parsed;

        };
#pragma region ConversionShit
        template <class T>
        std::pair<bool, T> convert(const std::string& str)
        {
            std::istringstream iss(str);
            T dummy;
            iss >> dummy;
            return { iss && iss.eof(),dummy };
        }

        template <class T>
        auto convert_stream(std::istringstream* iss)
        {
            T dummy;
            (*iss) >> dummy;
            return dummy;
        }


        template <class T, unsigned long N>
        struct convert_tuple {};

        template <class T>
        struct convert_tuple<T, 2>
        {

            static auto convert(const std::string& str) ->decltype(auto)
            {
                std::istringstream iss(str);
                std::tuple<T, T> results = { convert_stream<T>(&iss),convert_stream<T>(&iss) };
                return std::tuple<bool, std::tuple<T, T>>{iss&& iss.eof(), results};
            }
        };
        template <class T>
        struct convert_tuple<T, 3>
        {

            static auto convert(const std::string& str) ->decltype(auto)
            {
                std::istringstream iss(str);
                std::tuple<T, T, T> results = { convert_stream<T>(&iss),convert_stream<T>(&iss),convert_stream<T>(&iss) };
                return std::tuple<bool, std::tuple<T, T, T>>{iss&& iss.eof(), results};
            }
        };
        template <class T>
        struct convert_tuple<T, 4>
        {

            static auto convert(const std::string& str) ->decltype(auto)
            {
                std::istringstream iss(str);
                std::tuple<T, T, T, T> results = { convert_stream<T>(&iss),convert_stream<T>(&iss),convert_stream<T>(&iss),convert_stream<T>(&iss) };
                return std::tuple<bool, std::tuple<T, T, T, T>>{iss&& iss.eof(), results};
            }
        };
#pragma endregion

    }


    //material_handle | material: the material handle for the material that will have its paramters set.
    //std::string | section: the section of the ini file that we are going to parse.
    //fs::view | file: the path to the ini file we are parsing.
    inline void apply_material_conf(material_handle material, std::string section, fs::view file)
    {
        detail::handler_to_cpp handler;

        auto str = file.get().decay().to_string();
        const char* cstr = str.c_str();
        ini_parse_string(cstr, &detail::handler_to_cpp::handle, &handler);//parses the ini data into a usable form.

      //  ini_parse_stream(&detail::bytes_to_ini::reader,&reader,&detail::handler_to_cpp::handle,&handler);

        //This function goes through each value in a section, reads it, converts it to an appropriate data type and sets the materials params.
        handler.for_each_value_in_section(std::move(section), [&material, &file](std::string key, std::string value)
            {
                //these first two are for bools
                if (value == "true")
                {
                    material.set_param(key, true);
                    return;
                }
                else if (value == "false")
                {
                    material.set_param(key, false);
                    return;
                }
                else if (value.find_first_of(".f") != std::string::npos) //checks if value is a vector with dimensions 1-4 (where a vector1 is a float), and then converts the value into its respective type
                {//.f stands for float
                    if (auto [success, values] = detail::convert_tuple<float, 4>::convert(value); success)
                    {
                        auto& [v1, v2, v3, v4] = values;
                        material.set_param(key, math::vec4(v1, v2, v3, v4));
                        return;
                    }
                    if (auto [success, values] = detail::convert_tuple<float, 3>::convert(value); success)
                    {
                        auto& [v1, v2, v3] = values;
                        material.set_param(key, math::vec3(v1, v2, v3));
                        return;
                    }
                    if (auto [success, values] = detail::convert_tuple<float, 2>::convert(value); success)
                    {
                        auto& [v1, v2] = values;
                        material.set_param(key, math::vec2(v1, v2));
                        return;
                    }
                    if (auto [success, parsed] = detail::convert<float>(value); success)
                    {
                        material.set_param(key, parsed);
                        return;
                    }
                }
                else//checks if value is a ivector with dimensions 1-4 (where a vector1 is a int), and hten converts the value into its respective type
                {
                    if (auto [success, values] = detail::convert_tuple<int, 4>::convert(value); success)
                    {
                        auto& [v1, v2, v3, v4] = values;
                        material.set_param(key, math::ivec4(v1, v2, v3, v4));
                        return;
                    }
                    if (auto [success, values] = detail::convert_tuple<int, 3>::convert(value); success)
                    {
                        auto& [v1, v2, v3] = values;
                        material.set_param(key, math::ivec3(v1, v2, v3));
                        return;
                    }
                    if (auto [success, values] = detail::convert_tuple<int, 2>::convert(value); success)
                    {
                        auto& [v1, v2] = values;
                        material.set_param(key, math::ivec2(v1, v2));
                        return;
                    }
                    if (auto [success, parsed] = detail::convert<int>(value); success)
                    {
                        material.set_param(key, parsed);
                        return;
                    }
                }
                material.set_param(key, rendering::TextureCache::create_texture(file.parent() / value));//this will only ever be called if the value is unrecognizable, then the parameter is set to the a texture handle
            });
    }

    inline void generate_material_conf()
    {

    }
}
