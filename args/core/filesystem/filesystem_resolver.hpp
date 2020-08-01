#pragma once

#include <string>                       // std::string
#include <set>                          // set::set

#include <core/platform/platform.hpp>   // ARGS_PURE, A_NODISCARD
#include <core/filesystem/resource.hpp> // basic_resource
#include <core/common/inteface_traits.hpp>
#include <core/common/result.hpp>


#include "detail/error.hpp"
#include "detail/traits.hpp"            // file_traits
#include "detail/strpath_manip.hpp"     // strpath_manip::seperator


/**
 * @file filesystem_resolver.hpp
 */

namespace args::core::filesystem{

    /**@class filesystem_resolver
     * @brief abstract class that should represent and implement all functions of your
     *        filesystem, note in particular the pure virtual functions
     */
    class filesystem_resolver {
    public:
        filesystem_resolver() = default;

        filesystem_resolver(const filesystem_resolver&) = default;
        filesystem_resolver(filesystem_resolver&&) noexcept = default;

        filesystem_resolver& operator=(const filesystem_resolver&) = default;
        filesystem_resolver& operator=(filesystem_resolver&&) = default;

        virtual ~filesystem_resolver() = default;

        /** @brief gets all traits of a particular file
         *  @note same as calling @ref set_target and then calling all the individual traits manually
         *  @param [in] path the location identifier to the resource
         *  @return file_traits all traits of the file bunched together
         */
        A_NODISCARD file_traits resolve(const std::string& path) noexcept;


        /** @brief sets the identifier of the filesystem 
         *  @note if this filesystem is nested it should be the path to the root of the fs otherwise it can be arbitrary
         *  @param [in] ident the identifier of the filesystem 
         */
        void set_identifier(const std::string& ident) { m_identifier = ident;}


        /** @brief sets the target path of the resolver
         *  @param [in] target the location identifier to the resource
         */
        void set_target(const std::string& target) { m_target = target; }

        /** @note (implementation details)
         *        certain traits obviously overrule others
         *        the progression here works like this
         *        - if is_file cannot be directory
         *        - if is_directory cannot be written and cannot be read
         *        - if exists cannot be created and is valid
         *        - if exists and is file can be read
         */

        /** @brief checks if target points to a file */
        A_NODISCARD virtual bool is_file()      const noexcept ARGS_PURE;
        /** @brief checks if target points to a directory */
        A_NODISCARD virtual bool is_directory() const noexcept ARGS_PURE;
        /** @brief checks if target points to a valid location */
        A_NODISCARD virtual bool is_valid()     const noexcept ARGS_PURE;
        /** @brief checks if the target location can be written to*/
        A_NODISCARD virtual bool writeable()    const noexcept ARGS_PURE;
         /** @brief checks if the target location can be read from*/
        A_NODISCARD virtual bool readable()     const noexcept ARGS_PURE;
         /** @brief checks if the target location can be created*/
        A_NODISCARD virtual bool creatable()    const noexcept ARGS_PURE;
         /** @brief checks if the target location exists*/
        A_NODISCARD virtual bool exists()       const noexcept ARGS_PURE;

        /** @brief gets all entries in a directory or similar concept (assume -a)
         *  @note when not is_directory this should return an empty set
         *  @ref set_target
         *  @return a set of strings of entries at the current pointed at location
         */
        A_NODISCARD virtual std::set<std::string> ls() const noexcept ARGS_PURE;

        /** @brief get the contents of the file pointed at
         *  @note when not readable this should return an fs_error using Err()
         *  @ref set_target
         *  @return basic_resource wrapped in Ok() or fs_error wrapped in Err()
         */
        A_NODISCARD common::result<basic_resource,fs_error> get() const noexcept;
        virtual common::result<basic_resource,fs_error> get(interfaces::implement_signal_t) const noexcept ARGS_PURE;

        /** @brief sets the contents of the file pointed at
         *  @note when not writeable should do nothing and return false
         *  @param [in] res the resource you want to set at location
         *  @ref set_target
         *  @return bool, false on failure
         */
        A_NODISCARD bool set(const basic_resource& res);
        virtual bool set(interfaces::implement_signal_t,const basic_resource& res) ARGS_PURE;

        /** @brief gets the delimiter of the filesystem
         *  @return char single character for the delimiter of the filesystem (default: strpath_manip::seperator())
         */
        A_NODISCARD virtual char get_delimiter() const noexcept ARGS_IMPURE_RETURN(strpath_manip::seperator())


        /** @brief destroys the file pointed to
         *  @note when the files does not exists (traits.valid = false) this has no effect
         */
        void erase() const noexcept;
        virtual void erase(interfaces::implement_signal_t) const noexcept ARGS_PURE;


    protected:
        A_NODISCARD const std::string & get_target() const { return m_target; }
        A_NODISCARD const filesystem_traits& get_traits() const { return m_traits; }

        A_NODISCARD const std::string& get_identifier() const { return m_identifier; }

    private:
        std::string m_identifier;
        std::string m_target;
        filesystem_traits m_traits;
            
    };
}
