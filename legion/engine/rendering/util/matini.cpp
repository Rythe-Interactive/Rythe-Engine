#include <core/core.hpp>
#include <string>
#include <rendering/data/material.hpp>
#include <rendering/util/matini.hpp>

namespace legion::rendering {

    void apply_material_conf(material_handle& material, const std::string& section, fs::view file)
    {
        detail::handler_to_cpp handler;

        const auto str = file.get().except([](auto err)
            {
                log::warn("Unable to open {}, could not load ini settings!");
                return fs::basic_resource("");
            }).to_string();
            const char* const cstr = str.c_str();
            ini_parse_string(cstr, &detail::handler_to_cpp::handle, &handler);//parses the ini data into a usable form.


            //This function goes through each value in a section, reads it, converts it to an appropriate data type and sets the materials params.
            handler.for_each_value_in_section(section, [&material, &file](std::string key, std::string value)
                {
                    //these first two are for bools
                    if (value == "true")
                    {
                        material.set_param(key, true);
                        return false;
                    }
                    else if (value == "false")
                    {
                        material.set_param(key, false);
                        return false;
                    }
                    else if (value.find_first_of(".f") != std::string::npos) //checks if value is a vector with dimensions 1-4 (where a vector1 is a float), and then converts the value into its respective type
                    {//.f stands for float
                        if (auto [success, values] = detail::convert_tuple<float, 4>::convert(value); success)
                        {
                            auto& [v1, v2, v3, v4] = values;
                            material.set_param(key, math::vec4(v1, v2, v3, v4));
                            return false;
                        }
                        if (auto [success, values] = detail::convert_tuple<float, 3>::convert(value); success)
                        {
                            auto& [v1, v2, v3] = values;
                            material.set_param(key, math::vec3(v1, v2, v3));
                            return false;
                        }
                        if (auto [success, values] = detail::convert_tuple<float, 2>::convert(value); success)
                        {
                            auto& [v1, v2] = values;
                            material.set_param(key, math::vec2(v1, v2));
                            return false;
                        }
                        if (auto [success, parsed] = detail::convert<float>(value); success)
                        {
                            material.set_param(key, parsed);
                            return false;
                        }
                    }
                    else//checks if value is a ivector with dimensions 1-4 (where a vector1 is a int), and hten converts the value into its respective type
                    {
                        if (auto [success, values] = detail::convert_tuple<int, 4>::convert(value); success)
                        {
                            auto& [v1, v2, v3, v4] = values;
                            material.set_param(key, math::ivec4(v1, v2, v3, v4));
                            return false;
                        }
                        if (auto [success, values] = detail::convert_tuple<int, 3>::convert(value); success)
                        {
                            auto& [v1, v2, v3] = values;
                            material.set_param(key, math::ivec3(v1, v2, v3));
                            return false;
                        }
                        if (auto [success, values] = detail::convert_tuple<int, 2>::convert(value); success)
                        {
                            auto& [v1, v2] = values;
                            material.set_param(key, math::ivec2(v1, v2));
                            return false;
                        }
                        if (auto [success, parsed] = detail::convert<int>(value); success)
                        {
                            material.set_param(key, parsed);
                            return false;
                        }
                    }
                    //this will only ever be called if the value is unrecognizable, then the parameter is set to the a texture handle
               
                    material.set_param(key, TextureCache::create_texture(fs::view(value)));
                    return false;
                });
    }
}
