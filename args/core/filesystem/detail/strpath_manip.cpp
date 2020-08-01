#include "strpath_manip.hpp"

#include <core/common/string_extra.hpp>

#define SEP_WINDOWS 1
#define SEP_NIX 2

#ifdef _WIN32
#define OSSEP SEP_WINDOWS
#else
#define OSSEP SEP_NIX
#endif


namespace args::core::filesystem
{
	std::string strpath_manip::parent(const std::string& p)
	{
	    const auto sub = p.find_last_of("/\\",p.size()-2);
	    return p.substr(0,sub);
	}

	std::string strpath_manip::subdir(const std::string& p, const std::string& sub)
	{
		return p + seperator() + sub;
	}

	std::string strpath_manip::sanitize(const std::string& p)
	{

		std::vector<std::string> recreation;
		auto tokens = args::core::common::split_string_at<'\\','/'>(p);
		for(const auto& token : tokens)
		{
			if(common::rtrim_copy(token) == "..")
			{
				if(!recreation.empty() && recreation.back() != "..")
				{
					recreation.pop_back();
				}
				else recreation.emplace_back("..");
			}
			else if(common::rtrim_copy(token) != ".")
			{
				recreation.push_back(token);
			}
		}
		return args::core::common::join_strings_with<std::vector<std::string>,std::string>(recreation,seperator());
	}


	std::string strpath_manip::localize(const std::string& p)
	{
		std::string scopy = p;
		std::replace(begin(scopy),end(scopy),anti_seperator(),seperator());
		return scopy;
	}


	std::string& strpath_manip::inline_localize(std::string& p)
	{
		std::replace(begin(p),end(p),anti_seperator(),seperator());
		return p;
	}

	constexpr char strpath_manip::seperator()
	{
		if constexpr (OSSEP == SEP_WINDOWS)
			return '\\';
		else return '/';
	}


	constexpr char strpath_manip::anti_seperator()
	{
		if constexpr (OSSEP == SEP_NIX)
			return '\\';
		else return '/';
	}
}

#undef OSSEP
#undef SEP_NIX
#undef SEP_WINDOWS
