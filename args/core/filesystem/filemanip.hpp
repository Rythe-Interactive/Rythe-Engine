#pragma once

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <string_view> // std::string_view
#include <memory> //std::unique_ptr
#include <cassert> //assert
#include <cstdio>


namespace args::core::filesystem {
	/**@brief Open File in binary mode and write to buffer
	 *
	 * @param [in] path the path of the file to open
	 * @return a vector of bytes with the contents of the file at path
	 */
    A_NODISCARD inline  byte_vec read_file(std::string_view path)
	{
		std::unique_ptr<FILE,decltype(&fclose)> file(
			fopen(std::string(path).c_str(),"r+b"),
			fclose
		);

		assert(file);

		fseek(file.get(),0L,SEEK_END);
		byte_vec container(ftell(file.get()));
		fseek(file.get(),0L,SEEK_SET);

		fread(container.data(),sizeof(byte_t),container.size(),file.get());

		return container;
	}

	/**@brief Open file in binary mode to write the buffer to it
	 *
	 * @param [in] path the path of the file you want to write to
	 * @param [in] container the buffer you want to write to the file
	 */
	inline void write_file(std::string_view path,const byte_vec& container)
	{
		std::unique_ptr<FILE,decltype(&fclose)> file(
			fopen(std::string(path).c_str(),"wb"),
			fclose
		);

		assert(file);

		fwrite(container.data(),sizeof(byte_t),container.size(),file.get());

	}

	namespace literals
	{
		byte_vec operator""_readfile(const char * str, std::size_t len)
		{
			return ::args::core::filesystem::read_file(std::string_view(str,len));
		}

    	auto operator""_writefile(const char * str,std::size_t len)
		{
			return [path=std::string_view(str,len)](const byte_vec& container)
			{
				write_file(path,container);
			};
		}

    	
	}
	
}