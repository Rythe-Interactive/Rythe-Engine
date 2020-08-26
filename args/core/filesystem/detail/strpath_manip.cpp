#include "strpath_manip.hpp"

#include <core/common/string_extra.hpp>



namespace args::core::filesystem
{
	std::string strpath_manip::parent(const std::string& p)
	{
	    const auto sub = p.find_last_of("/\\",p.size()-2);
	    return p.substr(0,sub);
	}

	std::string strpath_manip::subdir(const std::string& p, const std::string& sub)
	{
        return common::rtrim_copy(p,[](char c){ return c == separator() || c == anti_separator(); }) + separator() + sub;
	}

	std::string strpath_manip::sanitize(const std::string& p, bool fail_on_fs_leave)
	{

		std::vector<std::string> recreation;

        //tokenize
		auto tokens = args::core::common::split_string_at<'\\','/'>(p);

        std::string filesystem;
		for(const auto& token : tokens)
		{
            //skip filesystem decl
            if(token.find(':') != std::string::npos)
            {
                filesystem = token + separator() + separator();
                continue;
            }

            //skip empty
            if(token.empty())
            {
                continue;
            }

            //handle upwards
			if(common::rtrim_copy(token) == "..")
			{
				if(!recreation.empty() && recreation.back() != "..")
				{
					recreation.pop_back();
				}
                else if(fail_on_fs_leave)
                {
                    //someone is trying something fishy! terminate parsing
                    return "";   
                }
				else recreation.emplace_back("..");
			}

            //handle normal tokens and .
			else if(common::rtrim_copy(token) != ".")
			{
				recreation.push_back(token);
			}
		}

        //reassemble string
		return filesystem + args::core::common::join_strings_with<std::vector<std::string>,std::string>(recreation,separator());
	}


	std::string strpath_manip::localize(const std::string& p)
	{
		std::string scopy = p;
		std::replace(begin(scopy),end(scopy),anti_separator(),separator());
		return scopy;
	}


	std::string& strpath_manip::inline_localize(std::string& p)
	{
		std::replace(begin(p),end(p),anti_separator(),separator());
		return p;
	}

}

