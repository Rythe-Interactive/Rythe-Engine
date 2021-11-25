#pragma once
#define _CRT_SECURE_NO_WARNINGS       // fopen vs fopen_s

#include <core/platform/platform.hpp> // L_NODISCARD
#include <core/types/types.hpp>       // byte, byte_vec
#include <core/common/assert.hpp>     // assert_msg

#include <string_view>                // std::string_view
#include <memory>                     // std::unique_ptr
#include <cstdio>                     // fopen, fclose, fseek, ftell, fread, fwrite

#include <fstream>

namespace legion::core::filesystem {


    /**@brief Check if file exists.
     * @param [in] path The path of the file to check.
     */
    L_NODISCARD inline bool exists(std::string_view path)
    {
        FILE* f = fopen(std::string(path).c_str(),"r+b");

        if(f)
        {
            fclose(f);
            return true;
        }
        return false;
    }

    /**@brief Open File in binary mode and write to buffer.
     *
     * @param [in] path The path of the file to open.
     * @return A vector of bytes with the contents of the file at path.
     */
    L_NODISCARD inline byte_vec read_file(std::string_view path)
    {
        std::ifstream file(path.data(), std::ios::ate | std::ios::binary);

        assert_msg("could not open file", file.is_open());

        size_t fileSize = (size_t)file.tellg();
        byte_vec buffer(fileSize);

        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

        file.close();

        return buffer;
    }

    /**@brief Open file in binary mode to write the buffer to it.
     *
     * @param [in] path The path of the file you want to write to.
     * @param [in] container The buffer you want to write to the file.
     */
    inline void write_file(std::string_view path,const byte_vec& container)
    {

        //create managed FILE ptr
        const std::unique_ptr<FILE,decltype(&fclose)> file(
            fopen(std::string(path).c_str(),"wb"),
            fclose
        );

        assert_msg("could not open file",file);

        // read data
        fwrite(container.data(),sizeof(byte),container.size(),file.get());

    }


    /**@brief Work the same as the above, but the path
     *  parameter is replaced by the string literal.
     */
    namespace literals
    {
        byte_vec LEGION_FUNC operator""_readfile(const char* str, std::size_t len);

        auto LEGION_FUNC operator""_writefile(const char* str, std::size_t len);

        bool LEGION_FUNC operator""_exists(const char* str, std::size_t len);
    }
}

#undef _CRT_SECURE_NO_WARNINGS

 //code example
/*
void example()
{
    using namespace  legion::core::fs::literals;

    auto file1 = "hello_world.cpp"_readfile;
    auto file2 = legion::core::fs::read_file("hello_world.cpp");

    "hello_world2.cpp"_writefile(file2);
    legion::core::fs::write_file("hello_world2.cpp",file1);
}
*/
