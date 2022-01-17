#pragma once
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <optional>
#include <functional>
#include <core/platform/platform.hpp>
#include <core/common/exception.hpp>

namespace legion::core::common
{
    struct valid_t {};
    static constexpr valid_t valid{};

    struct error_t {};
    static constexpr error_t error{};

    struct success_t {};
    static constexpr success_t success{};

    template<typename Success, typename Error = exception, typename Warning = std::string>
    struct result
    {
        using success_type = Success;
        using error_type = Error;
        using warning_type = Warning;
        using warning_list = std::vector<warning_type>;

        result(success_type&& s) : m_success(s), m_succeeded(true) {}
        result(const success_type& s) : m_success(s), m_succeeded(true) {}
        result(success_type&& s, warning_list&& w) : m_success(s), m_succeeded(true), m_warnings(w) {}
        result(success_type&& s, const warning_list& w) : m_success(s), m_succeeded(true), m_warnings(w) {}
        result(const success_type& s, warning_list&& w) : m_success(s), m_succeeded(true), m_warnings(w) {}
        result(const success_type& s, const warning_list& w) : m_success(s), m_succeeded(true), m_warnings(w) {}

        result(error_type&& e) : m_error(e), m_handled(false), m_succeeded(false) {}
        result(const error_type& e) : m_error(e), m_handled(false), m_succeeded(false) {}
        result(error_type&& e, warning_list&& w) : m_error(e), m_handled(false), m_succeeded(false), m_warnings(w) {}
        result(error_type&& e, const warning_list& w) : m_error(e), m_handled(false), m_succeeded(false), m_warnings(w) {}
        result(const error_type& e, warning_list&& w) : m_error(e), m_handled(false), m_succeeded(false), m_warnings(w) {}
        result(const error_type& e, const warning_list& w) : m_error(e), m_handled(false), m_succeeded(false), m_warnings(w) {}

        result(const result& src) : m_handled(src.m_handled), m_succeeded(src.m_succeeded), m_warnings(src.m_warnings)
        {
            if (src.m_succeeded)
                new (&m_success) success_type(src.m_success);
            else
                new (&m_error) error_type(src.m_error);
        }

        result(result&& src) : m_handled(src.m_handled), m_succeeded(src.m_succeeded), m_warnings(std::move(src.m_warnings))
        {
            src.mark_handled();
            if (src.m_succeeded)
                new (&m_success) success_type(std::move(src.m_success));
            else
                new (&m_error) error_type(std::move(src.m_error));
        }

        ~result() noexcept(false)
        {
            m_warnings.~vector();

            if (m_succeeded)
                m_success.~success_type();
            else if (!m_handled)
                throw m_error;
        }

        L_NODISCARD success_type& value()
        {
            if (m_succeeded)
                return m_success;
            throw m_error;
        }

        L_NODISCARD const success_type& value() const
        {
            if (m_succeeded)
                return m_success;
            throw m_error;
        }

        L_NODISCARD operator bool() const noexcept { return m_succeeded; }
        L_NODISCARD bool operator ==(const valid_t&) const noexcept { return m_succeeded; }
        L_NODISCARD bool operator !=(const valid_t&) const noexcept { return !m_succeeded; }
        L_NODISCARD bool valid() const noexcept { return m_succeeded; }
        L_NODISCARD bool has_error() const noexcept { return !m_succeeded; }

        L_NODISCARD const error_type& error() const
        {
            if (!m_succeeded)
            {
                m_handled = true;
                return m_error;
            }
            throw std::runtime_error("this result would have been valid!");
        }

        L_NODISCARD error_type& error()
        {
            if (!m_succeeded)
            {
                m_handled = true;
                return m_error;
            }
            throw std::runtime_error("this result would have been valid!");
        }

        void mark_handled() const noexcept { m_handled = true; }

        L_NODISCARD success_type& operator*() { return m_success; }
        L_NODISCARD const success_type& operator*() const { return m_success; }
        L_NODISCARD success_type* operator->() { return &m_success; }
        L_NODISCARD const success_type* operator->() const { return &m_success; }

        L_NODISCARD bool has_warnings() const noexcept { return !m_warnings.empty(); }
        L_NODISCARD size_t warning_count() const noexcept { return m_warnings.size(); }
        L_NODISCARD const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        L_NODISCARD Warning& warning_at(size_t i) { return m_warnings[i]; }
        L_NODISCARD const warning_list& warnings() const { return m_warnings; }
        L_NODISCARD warning_list& warnings() { return m_warnings; }

        template<typename Func, typename... Args>
        auto except(Func&& f, Args&&... args)
        {
            if (!m_succeeded)
            {
                m_handled = true;
                return std::invoke(std::forward<Func>(f), m_error, std::forward<Args>(args)...);
            }
            return m_success;
        }

    private:
        union
        {
            success_type m_success;
            error_type m_error;
        };
        bool m_succeeded : 1;
        mutable bool m_handled : 1;
        warning_list m_warnings;
    };

    template<typename Error, typename Warning>
    struct result<void, Error, Warning>
    {
        using success_type = void;
        using error_type = Error;
        using warning_type = Warning;
        using warning_list = std::vector<warning_type>;

        result(const result& src) : m_handled(src.m_handled), m_error(src.m_error), m_warnings(src.m_warnings) {}
        result(result&& src) : m_handled(src.m_handled), m_error(src.m_error), m_warnings(src.m_warnings) { src.m_handled = true; }
        ~result() noexcept(false)
        {
            if (m_error && !m_handled)
                throw* m_error;
        }

        result& operator=(const result& src)
        {
            if (m_error && !m_handled)
                throw* m_error;

            m_handled = src.m_handled;
            m_error = src.m_error;
            m_warnings = src.m_warnings;
        }

        result& operator=(result&& src)
        {
            if (m_error && !m_handled)
                throw* m_error;

            m_handled = src.m_handled;
            m_error = src.m_error;
            m_warnings = src.m_warnings;
            src.m_handled = true;
        }

        result(success_t) {}
        result(success_t, warning_list&& w) : m_warnings(w) {}
        result(success_t, const warning_list& w) : m_warnings(w) {}

        result(error_type&& e) : m_error(e) {}
        result(const error_type& e) : m_error(e) {}
        result(error_type&& e, warning_list&& w) : m_error(e), m_warnings(w) {}
        result(error_type&& e, const warning_list& w) : m_error(e), m_warnings(w) {}
        result(const error_type& e, warning_list&& w) : m_error(e), m_warnings(w) {}
        result(const error_type& e, const warning_list& w) : m_error(e), m_warnings(w) {}

        L_NODISCARD operator bool() const noexcept { return !m_error; }
        L_NODISCARD bool operator ==(const valid_t&) const noexcept { return !m_error; }
        L_NODISCARD bool operator !=(const valid_t&) const noexcept { return m_error.has_value(); }
        L_NODISCARD bool valid() const noexcept { return !m_error; }
        L_NODISCARD bool has_error() const noexcept { return m_error.has_value(); }

        L_NODISCARD const error_type& error() const
        {
            if (m_error)
            {
                m_handled = true;
                return *m_error;
            }
            throw std::runtime_error("this result would have been valid!");
        }

        L_NODISCARD error_type& error()
        {
            if (m_error)
            {
                m_handled = true;
                return *m_error;
            }
            throw std::runtime_error("this result would have been valid!");
        }

        void mark_handled() const noexcept { m_handled = true; }

        L_NODISCARD bool has_warnings() const noexcept { return !m_warnings.empty(); }
        L_NODISCARD size_t warning_count() const noexcept { return m_warnings.size(); }
        L_NODISCARD const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        L_NODISCARD Warning& warning_at(size_t i) { return m_warnings[i]; }
        L_NODISCARD const warning_list& warnings() const { return m_warnings; }
        L_NODISCARD warning_list& warnings() { return m_warnings; }

        template<typename Func, typename... Args>
        void except(Func&& f, Args&&... args)
        {
            if (m_error)
            {
                m_handled = true;
                std::invoke(std::forward<Func>(f), *m_error, std::forward<Args>(args)...);
            }
        }

    private:
        mutable bool m_handled = false;
        std::optional<error_type> m_error;
        warning_list m_warnings;
    };

    template<typename Success, typename Warning>
    struct result<Success, void, Warning>
    {
        using success_type = Success;
        using error_type = void;
        using warning_type = Warning;
        using warning_list = std::vector<warning_type>;

        result(const result& src) = default;
        result(result&& src) = default;
        ~result() = default;

        result(error_t) {}
        result(error_t, warning_list&& w) : m_warnings(w) {}
        result(error_t, const warning_list& w) : m_warnings(w) {}

        result(success_type&& s) : m_success(s) {}
        result(const success_type& s) : m_success(s) {}
        result(success_type&& s, warning_list&& w) : m_success(s), m_warnings(w) {}
        result(success_type&& s, const warning_list& w) : m_success(s), m_warnings(w) {}
        result(const success_type& s, warning_list&& w) : m_success(s), m_warnings(w) {}
        result(const success_type& s, const warning_list& w) : m_success(s), m_warnings(w) {}

        L_NODISCARD operator bool() const noexcept { return m_success; }
        L_NODISCARD bool operator ==(const valid_t&) const noexcept { return m_success; }
        L_NODISCARD bool operator !=(const valid_t&) const noexcept { return !m_success; }
        L_NODISCARD bool valid() const noexcept { return m_success; }
        L_NODISCARD bool has_error() const noexcept { return !m_success; }

        L_NODISCARD const success_type& value() const
        {
            if (m_success) return *m_success;
            throw legion_exception_msg("this result is invalid!");
        }

        L_NODISCARD success_type& value()
        {
            if (m_success) return *m_success;
            throw legion_exception_msg("this result is invalid!");
        }

        L_NODISCARD success_type& operator*() { return *m_success; }
        L_NODISCARD const success_type& operator*() const { return *m_success; }
        L_NODISCARD success_type* operator->() { return &*m_success; }
        L_NODISCARD const success_type* operator->() const { return &*m_success; }

        L_NODISCARD bool has_warnings() const noexcept { return !m_warnings.empty(); }
        L_NODISCARD size_t warning_count() const noexcept { return m_warnings.size(); }
        L_NODISCARD const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        L_NODISCARD Warning& warning_at(size_t i) { return m_warnings[i]; }
        L_NODISCARD const warning_list& warnings() const { return m_warnings; }
        L_NODISCARD warning_list& warnings() { return m_warnings; }

        template<typename Func, typename... Args>
        auto except(Func&& f, Args&&... args)
        {
            if (!m_success)
                return std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
            return *m_success;
        }

    private:
        std::optional<success_type> m_success;
        warning_list m_warnings;
    };

    template<typename Warning>
    struct result<void, void, Warning>
    {
        using success_type = void;
        using error_type = void;
        using warning_type = Warning;
        using warning_list = std::vector<warning_type>;

        constexpr result(bool succeeded) noexcept : m_succeeded(succeeded) {};
        constexpr result(error_t) noexcept : m_succeeded(false) {};
        constexpr result(success_t) noexcept : m_succeeded(true) {};

        result(const result& src) = default;
        result(result&& src) = default;
        ~result() = default;

        result(bool succeeded, warning_list&& w) : m_succeeded(succeeded), m_warnings(w) {}
        result(bool succeeded, const warning_list& w) : m_succeeded(succeeded), m_warnings(w) {}

        result(error_t, warning_list&& w) : m_succeeded(false), m_warnings(w) {}
        result(error_t, const warning_list& w) : m_succeeded(false), m_warnings(w) {}

        result(success_t, warning_list&& w) : m_succeeded(true), m_warnings(w) {}
        result(success_t, const warning_list& w) : m_succeeded(true), m_warnings(w) {}

        L_NODISCARD operator bool() const noexcept { return m_succeeded; }
        L_NODISCARD bool operator ==(const valid_t&) const noexcept { return m_succeeded; }
        L_NODISCARD bool operator !=(const valid_t&) const noexcept { return !m_succeeded; }
        L_NODISCARD bool valid() const noexcept { return m_succeeded; }
        L_NODISCARD bool has_error() const noexcept { return !m_succeeded; }

        L_NODISCARD bool has_warnings() const noexcept { return !m_warnings.empty(); }
        L_NODISCARD size_t warning_count() const noexcept { return m_warnings.size(); }
        L_NODISCARD const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        L_NODISCARD Warning& warning_at(size_t i) { return m_warnings[i]; }
        L_NODISCARD const warning_list& warnings() const { return m_warnings; }
        L_NODISCARD warning_list& warnings() { return m_warnings; }


        template<typename Func, typename... Args>
        void except(Func&& f, Args&&... args)
        {
            if (!m_succeeded)
                std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
        }

    private:
        bool m_succeeded;
        warning_list m_warnings;
    };

#define PropagateErrors(result, warnings)                                                      \
{                                                                                           \
    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());    \
    if (result.has_error()) { return { result.error(), warnings }; }                        \
}

}
