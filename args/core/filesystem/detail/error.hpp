#pragma once
#include <exception>
#include <string>
#include <utility>


namespace args::core::filesystem
{
	class fs_error : std::exception
	{
	public:
		fs_error(const fs_error& other) = default;
		fs_error(fs_error&& other) noexcept = default;
		fs_error& operator=(const fs_error& other) = default;
		fs_error& operator=(fs_error&& other) noexcept = default;

		explicit fs_error(const char * message) :
			m_message(message){}

		explicit fs_error(std::string message):
			m_message(std::move(message)){}

		virtual ~fs_error() noexcept = default;

		[[nodiscard]] const char * what() const noexcept override
		{
			return m_message.c_str();
		}
		
	protected:
		std::string m_message;
	};
}
