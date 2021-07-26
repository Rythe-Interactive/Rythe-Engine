#pragma once

#include <core/common/result.hpp>
#include <core/filesystem/resource.hpp>
#include <core/filesystem/basic_resolver.hpp>

#include <core/common/exception.hpp>

#include <Optick/optick.h>

#include "mem_filesystem_resolver.hpp"
#include "navigator.hpp"
#include "detail/traits.hpp"


namespace legion::core::filesystem
{
    /**@class view
     * @brief Generates a view onto the virtual filesystem,
     *        can be used to do all kinds of fun stuff with,
     *        the underlying drivers.
     */
    class view
    {
    public:

        //behold the noise that is copy and move operations
        view(const view& other) = default;
        view(view&& other) noexcept = default;
        view& operator=(const view& other) = default;
        view& operator=(view&& other) noexcept = default;
        virtual ~view() = default;

        view(std::string_view path) : m_path(strpath_manip::localize(std::string(path))) {}

        /** @brief Checks the view for validity.
         *  @note No deep check!
         */
        L_NODISCARD operator bool() const;

        /** @brief Checks the view for validity
         *  @param deep_check Also checks if the provided path can be resolved
         *         instead of doing just a basic sanity check.
         */
        L_NODISCARD bool is_valid( bool deep_check = false) const;

        /** @brief Gets the traits of the file pointed to.
         */
        L_NODISCARD file_traits file_info() const;

        /** @brief Gets the traits of the top most filesystem pointed to.
         */
        L_NODISCARD filesystem_traits filesystem_info() const;

        /** @brief Gets the root domain.
         */
        L_NODISCARD std::string get_domain() const;

        /**@brief Gets full virtual path.
         */
        L_NODISCARD const std::string& get_virtual_path() const;

        /**@brief Gets file extension if applicable.
         *  @note You can use legion::common::valid to check for validity.
         */
        L_NODISCARD common::result<std::string, fs_error> get_extension() const;

        /**@brief Gets file name if applicable.
         *  @note You can use legion::common::valid to check for validity.
         */
        L_NODISCARD common::result<std::string, fs_error> get_filename() const;

        /**@brief Gets file name if applicable.
         *  @note You can use legion::common::valid to check for validity.
         */
        L_NODISCARD common::result<std::string, fs_error> get_filestem() const;

        /** @brief Gets the contents of the resource pointed to.
         *  @note You can use legion::common::valid to check for validity.
         */
        L_NODISCARD common::result<basic_resource,fs_error> get();
        L_NODISCARD common::result<const basic_resource,fs_error> get() const;


        /** @brief Sets the contents of the resource pointed to.
         *  @note When setting was not possible has_err() will be true and get_err().what() will contain information on what went wrong.
         */
        L_NODISCARD common::result<void,fs_error> set(const basic_resource& resource);

        /** @brief Gets the parent folder of the file/folder and creates a new view from it.
         */
        L_NODISCARD view parent() const;

        /** @brief Creates a new view from a sub path (by first joining them together and then sanitizing the input)
         **/
        L_NODISCARD virtual view find(std::string_view identifier) const;


        /** @brief alternative syntax for find
         */
        L_NODISCARD view operator[](std::string_view identifier) const;

        /** @brief same as get().value().to<T>()
         */
        template <class T, class... Args>
        auto load_as(Args&&...args) -> decltype(auto)
        {
            OPTICK_EVENT();
            return get().value().template to<T>(std::forward<Args>(args)...);
        }

        L_NODISCARD common::result<std::vector<view>,fs_error> ls() const;

#if  !defined( LEGION_DISABLE_POTENTIALLY_WEIRD_SYNTAX )

        /** @brief alternative syntax for find
         */
        L_NODISCARD view operator/(std::string_view identifier) const
        {
            return operator[](identifier);
        }

#endif

    protected:
        view() = default;
        std::string m_path;

        std::string create_identifier(const navigator::solution::iterator&) const;
        std::shared_ptr<filesystem_resolver> build() const;

        struct create_chain
        {
            std::shared_ptr<mem_filesystem_resolver> subject; // subject to be resolved
            std::shared_ptr<filesystem_resolver> provider;    // resolver to be used to resolve

            std::shared_ptr<create_chain> next; //next in chain
        };

        void make_inheritance() const;
        std::shared_ptr<create_chain> translate_solution() const;


        common::result<void,fs_error> make_solution() const;

        mutable navigator::solution m_foundSolution{};
    };

    class view_util
    {
    public:
        static std::string view_util::get_view_path(const view& view, bool mustBeFile);
    };


    namespace literals
    {
        /**@brief. creates a view from a string literal
         */
       inline view operator""_view(const char* str,std::size_t len)
       {
           return view(std::string_view(str,len));
       }
    }


#if 0 //not ready yet

    template <std::size_t S>
    class combined_view
    {
    public:

        template <class... Views,
                  typename = std::enable_if_t<(std::is_base_of_v<view,std::remove_reference<Views>> &&...)>>
        combined_view(Views&&...v) : m_views({std::forward<Views>(v)...}) {}

        view find(std::string_view identifier);

    private:
        std::array<view&,S> m_views;
    };

#if !defined(DOXY_EXCLUDE)
    template<class... Views>
    combined_view(Views...) -> combined_view<sizeof...(Views)>;
#endif

    template <std::size_t S>
    view combined_view<S>::find(std::string_view identifier)
    {
    }
#endif
}

