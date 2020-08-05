#include <core/filesystem/view.hpp>



#include "navigator.hpp"
#include "provider_registry.hpp"
#include "detail/strpath_manip.hpp"


namespace args::core::filesystem
{
    view::operator bool()
    {
        return is_valid();
    }

    bool view::is_valid( bool deep_check )
    {
        if(m_path.empty()) return false;
        if(!provider_registry::has_domain(get_domain())) return false;

        const navigator n(m_path);
        if(n.find_solution().has_err()) return false;


        return true;
    }

    file_traits view::file_info()
    {
        auto result = make_solution();

        //yes I know this is super ugly, but if you request a file that does not have a solution
        //it is not read/writable, does not exist is not valid or a directory etc...
        if(result.has_err()) return invalid_file_t;
        else
        {
            auto resolver = build();
            if(resolver == nullptr) return invalid_file_t;

        }

    }

    filesystem_traits view::filesystem_info()
    {
    }

    common::result_decay_more<basic_resource, fs_error> view::get()
    {
    }

    common::result<common::empty_t, fs_error> view::set(const basic_resource& resource)
    {
    }

    view view::parent()
    {
    }

    view view::find(std::string_view identifier)
    {
        std::string sanitized = strpath_manip::sanitize(std::string(identifier));

        if(sanitized == "..") return parent();
        if(sanitized == ".")  return *this;

        sanitized = strpath_manip::subdir(m_path,sanitized);
        sanitized = strpath_manip::sanitize(sanitized,true);

        return view(sanitized);
    }

    view view::operator[](std::string_view identifier)
    {
        return find(identifier);
    }

    common::result<void,fs_error> view::make_solution()
    {
        using common::Ok,common::Err;

        if(m_foundSolution.empty())
        {
            const navigator n(m_path);
            auto solution = n.find_solution();

            if(solution.has_err())
                return Err(solution.get_error());

            m_foundSolution = solution.get();
        }

        return Ok();
    }
}
