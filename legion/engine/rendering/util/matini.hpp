#pragma once

#include <inih/ini.h>
#include <tuple>
#include <sstream>
#include <vector>
namespace legion::rendering {

    struct material_handle;

    namespace detail
    {
        /**@class IniBuilder
         * @brief Bob the IniBuilder, a builder class to generate ini files
         * @note does not validate entries, thus invalid combinations like
         *       glyph("Something").value("v") is not caught as an error at runtime
         *       instead of
         *       glyph("Something").eq().value("v")
         */
        class IniBuilder
        {
        public:
            IniBuilder& glyph(const std::string& glyph)
            {
                m_contents += glyph;
                return *this;
            }

            IniBuilder& section(const std::string& v)   { return glyph("[" + v + "]\n"); }
            IniBuilder& eq()                            { return glyph("= "); }
            IniBuilder& comment(const std::string& v)   { return glyph("; " + v + "\n"); }

            IniBuilder& value(float v)                  { return glyph(std::to_string(v) + " "); }
            IniBuilder& value(int v)                    { return glyph(std::to_string(v) + " "); }
            IniBuilder& value(math::vec3 v)             { return value(v.x).value(v.y).value(v.z); }
            IniBuilder& value(math::ivec3 v)            { return value(v.x).value(v.y).value(v.z); }
            IniBuilder& value(math::vec4 v)             { return value(v.x).value(v.y).value(v.z).value(v.w); }
            IniBuilder& value(math::ivec4 v)            { return value(v.x).value(v.y).value(v.z).value(v.w); }
            IniBuilder& value(bool b)                   { return glyph(b ? "true " : "false "); }
            IniBuilder& value(const std::string& v)     { return glyph(v + " "); }

            IniBuilder& finish_entry()                  { return glyph("\n"); }

            IniBuilder& push_state()                    { m_copy = m_contents; return *this; }
            IniBuilder& pop_state()                     { m_contents = m_copy; return *this;}


            L_NODISCARD std::string get() const noexcept { return m_contents; }

        private:
            std::string m_contents;
            std::string m_copy;
        };

        class handler_to_cpp
        {
        public:
            //a place to store the data? maybe, not used
            static int handle(void* user, const char* section, const char* name, const char* value)
            {
                auto& self = *static_cast<handler_to_cpp*>(user);
                self.m_parsed.emplace(section, std::make_pair<std::string, std::string>(name, value));
                return 1;
            }

            //function to go through each value in a section
            template <class Func>
            void for_each_value_in_section(const std::string& section, Func&& f)
            {
                auto range = pair_range(m_parsed.equal_range(section));

                for (auto& [key, value] : values_only(range))
                {
                    if (std::invoke(f, key, value)) return;
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


    inline std::string extract_string(const std::string& section, const std::string& key, fs::view file)
    {
        detail::handler_to_cpp handler;

        const auto str = file.get().except([](auto err)
            {
                log::warn("Unable to open {}, could not load ini settings!");
                return fs::basic_resource("");
            }).to_string();

            const char* const cstr = str.c_str();
            ini_parse_string(cstr, &detail::handler_to_cpp::handle, &handler);//parses the ini data into a usable form.

            std::string v;

            handler.for_each_value_in_section(section, [&](const std::string& k, const std::string& value)
                {
                    if (k == key)
                    {
                        v = value;
                        return true;
                    }
                    return false;
                });
            return v;
    }


    //material_handle | material: the material handle for the material that will have its paramters set.
    //std::string | section: the section of the ini file that we are going to parse.
    //fs::view | file: the path to the ini file we are parsing.
    extern void apply_material_conf(material_handle& material, const std::string& section, fs::view file);

}
