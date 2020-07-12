#pragma once

#include <core/filesystem/filemanip.hpp>


//args::fs
namespace args::core
{
	namespace fs = ::args::core::filesystem;

}


 //code example
int main()
{
	using namespace  args::core::fs::literals;

	auto file1 = "hello_world.cpp"_readfile;

    auto file2 = args::core::fs::read_file("hello_world.cpp"); 

	"hello_world2.cpp"_writefile(file2);

	args::core::fs::write_file("hello_world2.cpp",file1);

}
