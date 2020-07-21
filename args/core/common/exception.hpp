#pragma once
#include <exception>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>

#define args_exception args::core::exception(__FILE__, __LINE__, __FULL_FUNC__)
#define args_exception_msg(msg) args::core::exception(msg, __FILE__, __LINE__, __FULL_FUNC__)
#define args_invalid_component_error args::core::invalid_component_error(__FILE__, __LINE__, __FULL_FUNC__)
#define args_invalid_component_msg(msg) args::core::invalid_component_error(msg, __FILE__, __LINE__, __FULL_FUNC__)
#define args_component_destroyed_error args::core::component_destroyed_error(__FILE__, __LINE__, __FUNC__)
#define args_component_destroyed_msg(msg) args::core::component_destroyed_error(msg, __FILE__, __LINE__, __FULL_FUNC__)

namespace args::core
{
	class exception : public std::exception
	{
	private:
		cstring file;
		uint line;
		cstring func;

	public:
		exception(cstring file, uint line, cstring func) : std::exception(), file(file), line(line), func(func) {}
		exception(cstring msg, cstring file, uint line, cstring func) : std::exception(msg), file(file), line(line), func(func) {}

		cstring get_file() const { return file; }
		uint get_line() const { return line; }
		cstring get_func() const { return func; }
	};

	class invalid_component_error : public exception
	{
	public:
		invalid_component_error(cstring file, uint line, cstring func) : exception("Component invalid.", file, line, func) {}
		invalid_component_error(cstring msg, cstring file, uint line, cstring func) : exception(msg, file, line, func) {}
	};

	class component_destroyed_error : public exception
	{
	public:
		component_destroyed_error(cstring file, uint line, cstring func) : exception("Component no longer exists.", file, line, func) {}
		component_destroyed_error(cstring msg, cstring file, uint line, cstring func) : exception(msg, file, line, func) {}
	};
}
