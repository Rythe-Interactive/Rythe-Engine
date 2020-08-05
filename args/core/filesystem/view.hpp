#pragma once
#include <core/common/result.hpp>
#include <core/filesystem/resource.hpp>


#include <core/common/exception.hpp>


#include "navigator.hpp"
#include "detail/traits.hpp"

namespace args::core::filesystem
{
    class view
    {
    public:

        //behold the noise that is copy and move operations
        view(const view& other) = default;
        view(view&& other) noexcept = default;
        view& operator=(const view& other) = default;
        view& operator=(view&& other) noexcept = default;
        virtual ~view() = default;

        view(std::string_view path) : m_path(path) {}

        A_NODISCARD operator bool();
        A_NODISCARD bool is_valid( bool deep_check = false);

        A_NODISCARD file_traits file_info();
        A_NODISCARD filesystem_traits filesystem_info();
        A_NODISCARD std::string get_domain();

        A_NODISCARD common::result_decay_more<basic_resource,fs_error> get();

        A_NODISCARD common::result<common::empty_t,fs_error> set(const basic_resource& resource);

        A_NODISCARD view parent();
        A_NODISCARD virtual view find(std::string_view identifier);

        A_NODISCARD view operator[](std::string_view identifier);

#if  !defined( ARGS_DISABLE_POTENTIALLY_WEIRD_SYNTAX )

        A_NODISCARD view operator/(std::string_view identifier)
        {
            return operator[](identifier);
        }

#endif

    protected:
        view() = default;
        const std::string m_path;

        filesystem_resolver* build();
        common::result<void,fs_error> make_solution();

        navigator::solution m_foundSolution;
    };

    template <std::size_t S>
    class combined_view : public view
    {
    public:

        template <class... Views,
                  typename = std::enable_if_t<(std::is_base_of_v<view,std::remove_reference<Views>> &&...)>>
        combined_view(Views&&...v) : m_views({std::forward<Views>(v)...}) {}

        view find(std::string_view identifier) override;

    private:
        std::array<view&,S> m_views;
    };

    template<class... Views>
    combined_view(Views...) -> combined_view<sizeof...(Views)>;

    template <std::size_t S>
    view combined_view<S>::find(std::string_view identifier)
    {
    }
}

