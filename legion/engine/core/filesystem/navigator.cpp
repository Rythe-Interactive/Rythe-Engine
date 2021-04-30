#include "navigator.hpp"
#include "detail/strpath_manip.hpp"
#include <core/common/string_extra.hpp>
#include <core/filesystem/provider_registry.hpp>

#include <Optick/optick.h>

namespace legion::core::filesystem {
    common::result<navigator::solution,fs_error> navigator::find_solution(const std::string& opt_root_domain) const 
    {
        OPTICK_EVENT();
        using common::Err,common::Ok;

        std::string root_domain;
        std::string to_process;

        if (!opt_root_domain.empty())
        {
            root_domain = opt_root_domain;
            to_process = m_path;
        }
        else {

            //find root domain
            // the syntax for inline root-domains has to be <root-domain>:[/\\]+<rest-of-the-path>
            const auto rdIndex = m_path.find_first_of(':');
            if (rdIndex == std::string::npos) return Err(legion_fs_error("invalid syntax for path string, no domain delimiter"));

            root_domain = m_path.substr(0, rdIndex);

            //find the rest of the path ... technically this will resolve //////////lol/test.bin to lol/test.bin just fine,
            //but hopefully no one is actually insane enough to mess with such paths
            //
            //if he/she does, congrats you found a ... "feature"
            const auto pIndex = m_path.find_first_not_of("/\\", rdIndex + 1);
            if (pIndex == std::string::npos)
            {
                to_process = std::string();
            }
            else {
                to_process = m_path.substr(pIndex, std::string::npos);
                to_process = strpath_manip::sanitize(to_process);
            }
        }

        if(root_domain.empty()) return Err(legion_fs_error("invalid syntax for path string, one or more properties empty"));
        root_domain += std::string(":") + strpath_manip::separator() + strpath_manip::separator();
        if(!provider_registry::has_domain(root_domain)) return Err(legion_fs_error(("no start! no such domain: " + root_domain).c_str()));

        solution steps{};

        auto tokens = common::split_string_at<'\\','/'>(to_process);

        std::string previous_domain = root_domain;
        std::string domain = root_domain;

        filesystem_resolver* resolver;
        std::string path_for_resolver;

        //get first resolver
        //TODO support multiple resolvers 
        resolver = *provider_registry::domain_get_first_resolver(domain);
        if (to_process.empty())
        {
            steps.emplace_back(resolver, "");
            return Ok(steps);
        }

        for (auto& token : tokens) {

            if(previous_domain != domain)
            {
                previous_domain = domain;

                if(!provider_registry::has_domain(domain)) return Err(legion_fs_error(("stop! no such domain: " + domain).c_str()));

                //add resolver step
                steps.emplace_back(resolver,strpath_manip::sanitize(path_for_resolver));

                //get new resolver
                resolver = *provider_registry::domain_get_first_resolver(domain);
                if(!dynamic_cast<memory_resolver_common_base*>(resolver)) return Err(legion_fs_error("sub domain resolver was not a mem resolver, illegal access"));

                path_for_resolver = "";
            }

            //we sanitized the string prior checking this
            //if .. or . still remains, than your path is simply wrong (relative paths to outside or a provider are not allowed!)
            if(token.find('.') != std::string::npos)
            {
                domain = token.substr(token.find_first_of('.'),std::string::npos);

                //the file itself is still part of the old resolver
                //i.e.: /sandbox/assets/archive.pp.xz -> directory_in_archive/file.png
                //where the new domain is ".pp.xz"
                path_for_resolver += token;
            }
            else 
            {
                path_for_resolver += token + resolver->get_delimiter();
            }
        }

        //add final step
        steps.emplace_back(resolver,strpath_manip::sanitize(path_for_resolver));

        return Ok(steps);
    }
}
