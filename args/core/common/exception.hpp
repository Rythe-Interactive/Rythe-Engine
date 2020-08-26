#pragma once
#include <exception>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>

/**
 * @file exception.hpp
 */

/**@def args_exception
 * @brief Args generic exception with file name, line number, and function name.
 */
#define args_exception args::core::exception(__FILE__, __LINE__, __FUNC__)

/**@def args_exception_msg
 * @brief Args generic exception with message, file name, line number, and function name.
 */
#define args_exception_msg(msg) args::core::exception(msg, __FILE__, __LINE__, __FUNC__)


#pragma region fetch
/**@def args_invalid_fetch_error
 * @brief Args generic fetch exception with file name, line number, and function name.
 */
#define args_invalid_fetch_error args::core::invalid_fetch_error(__FILE__, __LINE__, __FUNC__)

/**@def args_invalid_fetch_msg
 * @brief Args generic fetch exception with message, file name, line number, and function name.
 */
#define args_invalid_fetch_msg(msg) args::core::invalid_fetch_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_invalid_component_error
 * @brief Args invalid component fetch exception with file name, line number, and function name.
 */
#define args_invalid_component_error args::core::invalid_component_error(__FILE__, __LINE__, __FUNC__)

/**@def args_invalid_component_msg
 * @brief Args invalid component fetch exception with message, file name, line number, and function name.
 */
#define args_invalid_component_msg(msg) args::core::invalid_component_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_component_destroyed_error
 * @brief Args destroyed component fetch exception with file name, line number, and function name.
 */
#define args_component_destroyed_error args::core::component_destroyed_error(__FILE__, __LINE__, __FUNC__)

/**@def args_component_destroyed_msg
 * @brief Args destroyed component fetch exception with message, file name, line number, and function name.
 */
#define args_component_destroyed_msg(msg) args::core::component_destroyed_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_invalid_entity_error
 * @brief Args invalid entity fetch exception with file name, line number, and function name.
 */
#define args_invalid_entity_error args::core::invalid_entity_error(__FILE__, __LINE__, __FUNC__)
 
/**@def args_invalid_entity_msg
 * @brief Args invalid entity fetch exception with message, file name, line number, and function name.
 */
#define args_invalid_entity_msg(msg) args::core::invalid_entity_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_entity_not_found_error
 * @brief Args non-existent entity fetch exception with file name, line number, and function name.
 */
#define args_entity_not_found_error args::core::entity_not_found_error(__FILE__, __LINE__, __FUNC__)

/**@def args_entity_not_found_msg
 * @brief Args non-existent entity fetch exception with message, file name, line number, and function name.
 */
#define args_entity_not_found_msg(msg) args::core::entity_not_found_error(msg, __FILE__, __LINE__, __FUNC__)
#pragma endregion

#pragma region creation
/**@def args_invalid_creation_error
 * @brief Args generic creation exception with file name, line number, and function name.
 */
#define args_invalid_creation_error args::core::invalid_creation_error(__FILE__, __LINE__, __FUNC__)

/**@def args_invalid_creation_msg
 * @brief Args generic creation exception with message, file name, line number, and function name.
 */
#define args_invalid_creation_msg(msg) args::core::invalid_creation_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_entity_exists_error
 * @brief Args duplicate entity creation exception with file name, line number, and function name.
 */
#define args_entity_exists_error args::core::entity_exists_error(__FILE__, __LINE__, __FUNC__)

/**@def args_entity_exists_msg
 * @brief Args duplicate entity creation exception with message, file name, line number, and function name.
 */
#define args_entity_exists_msg(msg) args::core::entity_exists_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_component_exists_error
 * @brief Args duplicate component creation exception with file name, line number, and function name.
 */
#define args_component_exists_error args::core::component_exists_error(__FILE__, __LINE__, __FUNC__)

/**@def args_component_exists_msg
 * @brief Args duplicate component creation exception with message, file name, line number, and function name.
 */
#define args_component_exists_msg(msg) args::core::component_exists_error(msg, __FILE__, __LINE__, __FUNC__)
#pragma endregion

#pragma region type
/**@def args_invalid_type_error
 * @brief Args generic invalid type exception with file name, line number, and function name.
 */
#define args_invalid_type_error args::core::invalid_type_error(__FILE__, __LINE__, __FUNC__)

/**@def args_invalid_type_msg
 * @brief Args generic invalid type exception with message, file name, line number, and function name.
 */
#define args_invalid_type_msg(msg) args::core::invalid_type_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_unknown_component_error
 * @brief Args invalid component type exception with file name, line number, and function name.
 */
#define args_unknown_component_error args::core::unknown_component_error(__FILE__, __LINE__, __FUNC__)

/**@def args_unknown_component_msg
 * @brief Args invalid component type exception with message, file name, line number, and function name.
 */
#define args_unknown_component_msg(msg) args::core::unknown_component_error(msg, __FILE__, __LINE__, __FUNC__)


/**@def args_unknown_system_error
 * @brief Args invalid system type exception with file name, line number, and function name.
 */
#define args_unknown_system_error args::core::unknown_system_error(__FILE__, __LINE__, __FUNC__)

/**@def args_unknown_system_msg
 * @brief Args invalid system type exception with message, file name, line number, and function name.
 */
#define args_unknown_system_msg(msg) args::core::unknown_system_error(msg, __FILE__, __LINE__, __FUNC__)

/**@def args_fs_error
 * @brief Args filesystem Error with message, signifies something went wrong with
 *        requesting a resource from the virtual filesystem
 * @note catch args::core::fs_error if you need to filter for this exception
 */
#define args_fs_error(msg) args::core::fs_error(msg,__FILE__,__LINE__,__FUNC__)
#pragma endregion



namespace args::core
{
	/**@class exception
	 * @brief Args generic exception with file name, line number, and function name.
	 */
	class exception : public std::exception
	{
	private:
		cstring m_file;
		uint m_line;
		cstring m_func;
		cstring m_message;

	public:
		exception(cstring file, uint line, cstring func) : std::exception(), m_file(file), m_line(line), m_func(func), m_message("Args generic exception occurred.") {}
		exception(cstring msg, cstring file, uint line, cstring func) : std::exception(), m_file(file), m_line(line), m_func(func), m_message(msg) {}

		virtual cstring what() const noexcept override { return m_message; }
		cstring file() const noexcept { return m_file; }
		uint line() const noexcept { return m_line; }
		cstring func() const noexcept { return m_func; }
	};

#pragma region invalid fetch
	/**@class invalid_fetch_error
	 * @brief Args generic fetch exception.
	 */
	class invalid_fetch_error : public exception
	{
	public:
		invalid_fetch_error(cstring file, uint line, cstring func) : exception("Invalid fetch occurred.", file, line, func) {}
		invalid_fetch_error(cstring msg, cstring file, uint line, cstring func) : exception(msg, file, line, func) {}
	};

	class invalid_component_error : public invalid_fetch_error
	{
	public:
		invalid_component_error(cstring file, uint line, cstring func) : invalid_fetch_error("Component invalid.", file, line, func) {}
		invalid_component_error(cstring msg, cstring file, uint line, cstring func) : invalid_fetch_error(msg, file, line, func) {}
	};

	class component_destroyed_error : public invalid_component_error
	{
	public:
		component_destroyed_error(cstring file, uint line, cstring func) : invalid_component_error("Component no longer exists.", file, line, func) {}
		component_destroyed_error(cstring msg, cstring file, uint line, cstring func) : invalid_component_error(msg, file, line, func) {}
	};

	class invalid_entity_error : public invalid_fetch_error
	{
	public:
		invalid_entity_error(cstring file, uint line, cstring func) : invalid_fetch_error("Entity invalid.", file, line, func) {}
		invalid_entity_error(cstring msg, cstring file, uint line, cstring func) : invalid_fetch_error(msg, file, line, func) {}
	};

	class entity_not_found_error : public invalid_entity_error
	{
	public:
		entity_not_found_error(cstring file, uint line, cstring func) : invalid_entity_error("Entity does not exist.", file, line, func) {}
		entity_not_found_error(cstring msg, cstring file, uint line, cstring func) : invalid_entity_error(msg, file, line, func) {}
	};
#pragma endregion

#pragma region invalid creation
	/**@class invalid_creation_error
	 * @brief Args generic creation exception.
	 */
	class invalid_creation_error : public exception
	{
	public:
		invalid_creation_error(cstring file, uint line, cstring func) : exception("Creation invalid.", file, line, func) {}
		invalid_creation_error(cstring msg, cstring file, uint line, cstring func) : exception(msg, file, line, func) {}
	};

	class entity_exists_error : public invalid_creation_error
	{
	public:
		entity_exists_error(cstring file, uint line, cstring func) : invalid_creation_error("Entity already exist.", file, line, func) {}
		entity_exists_error(cstring msg, cstring file, uint line, cstring func) : invalid_creation_error(msg, file, line, func) {}
	};

	class component_exists_error : public invalid_creation_error
	{
	public:
		component_exists_error(cstring file, uint line, cstring func) : invalid_creation_error("Component already exist.", file, line, func) {}
		component_exists_error(cstring msg, cstring file, uint line, cstring func) : invalid_creation_error(msg, file, line, func) {}
	};
#pragma endregion

#pragma region invalid type
	/**@class invalid_type_error
	 * @brief Args generic type exception.
	 */
	class invalid_type_error : public exception
	{
	public:
		invalid_type_error(cstring file, uint line, cstring func) : exception("Type invalid.", file, line, func) {}
		invalid_type_error(cstring msg, cstring file, uint line, cstring func) : exception(msg, file, line, func) {}
	};

	class unknown_component_error : public invalid_type_error
	{
	public:
		unknown_component_error(cstring file, uint line, cstring func) : invalid_type_error("Unknown component type.", file, line, func) {}
		unknown_component_error(cstring msg, cstring file, uint line, cstring func) : invalid_type_error(msg, file, line, func) {}
	};

	class unknown_system_error : public invalid_type_error
	{
	public:
		unknown_system_error(cstring file, uint line, cstring func) : invalid_type_error("Unknown system type.", file, line, func) {}
		unknown_system_error(cstring msg, cstring file, uint line, cstring func) : invalid_type_error(msg, file, line, func) {}
	};
#pragma endregion

#pragma region filesystem
    class fs_error : public exception
    {
    public:
        fs_error(cstring file,uint line,cstring func) : exception("Filesystem Adapter failed", file,line,func) {}
        fs_error(cstring msg,cstring file,uint line,cstring func) : exception(msg,file,line,func) {}
    };
#pragma endregion 


}
