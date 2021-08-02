#pragma once
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <memory>
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

        result(error_type&& e) : m_error(e), m_succeeded(false) {}
        result(const error_type& e) : m_error(e), m_succeeded(false) {}
        result(error_type&& e, warning_list&& w) : m_error(e), m_succeeded(false), m_warnings(w) {}
        result(error_type&& e, const warning_list& w) : m_error(e), m_succeeded(false), m_warnings(w) {}
        result(const error_type& e, warning_list&& w) : m_error(e), m_succeeded(false), m_warnings(w) {}
        result(const error_type& e, const warning_list& w) : m_error(e), m_succeeded(false), m_warnings(w) {}

        result(const result& src) : m_succeeded(src.m_succeeded), m_warnings(src.m_warnings)
        {
            if (src.m_succeeded)
                m_success = src.m_succeeded;
            else
                m_error = src.m_error;
        }

        result(result&& src) : m_succeeded(std::move(src.m_succeeded)), m_warnings(std::move(src.m_warnings))
        {
            if (src.m_succeeded)
                m_success = std::move(src.m_succeeded);
            else
                m_error = std::move(src.m_error);
        }

        ~result()
        {
            if (m_succeeded)
                m_success.~success_type();
            else
                m_error.~error_type();

            m_warnings.~vector();
        }

        success_type&& value()
        {
            if (m_succeeded)
                return std::move(m_success);
            throw m_error;
        }

        const success_type& value() const
        {
            if (m_succeeded)
                return m_success;
            throw m_error;
        }

        operator bool() const noexcept { return m_succeeded; }
        bool operator ==(const valid_t&) const noexcept { return m_succeeded; }
        bool operator !=(const valid_t&) const noexcept { return !m_succeeded; }
        bool valid() const noexcept { return m_succeeded; }
        bool has_error() const noexcept { return !m_succeeded; }

        const error_type& error() const
        {
            if (!m_succeeded) return m_error;
            throw std::runtime_error("this result would have been valid!");
        }

        error_type&& error()
        {
            if (!m_succeeded) return std::move(m_error);
            throw std::runtime_error("this result would have been valid!");
        }

        operator success_type() { return value(); }
        operator success_type() const { return value(); }
        operator error_type() { return error(); }
        operator error_type() const { return error(); }

        bool has_warnings() const noexcept { return !m_warnings.empty(); }
        size_t warning_count() const noexcept { return m_warnings.size(); }
        const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        Warning& warning_at(size_t i) { return m_warnings[i]; }
        const warning_list& warnings() const { return m_warnings; }
        warning_list& warnings() { return m_warnings; }

        template<typename Func, typename... Args>
        auto except(Func&& f, Args&&... args) -> decltype(std::invoke(std::declval<Func>(), std::declval<error_type>(), std::declval<Args>()...))
        {
            if (!m_succeeded)
                return std::invoke(std::forward<Func>(f), m_error, std::forward<Args>(args)...);
            return m_success;
        }

    private:
        union
        {
            success_type m_success;
            error_type m_error;
        };
        bool m_succeeded;
        warning_list m_warnings;
    };

    template<typename Error, typename Warning>
    struct result<void, Error, Warning>
    {
        using success_type = void;
        using error_type = Error;
        using warning_type = Warning;
        using warning_list = std::vector<warning_type>;

        result(const result& src) = default;
        result(result&& src) = default;
        ~result() = default;

        result(success_t) {}
        result(success_t, warning_list&& w) : m_warnings(w) {}
        result(success_t, const warning_list& w) : m_warnings(w) {}

        result(warning_list&& w) : m_warnings(w) {}
        result(const warning_list& w) : m_warnings(w) {}

        result(error_type&& e) : m_error(std::make_unique<error_type>(e)) {}
        result(const error_type& e) : m_error(std::make_unique<error_type>(e)) {}
        result(error_type&& e, warning_list&& w) : m_error(std::make_unique<error_type>(e)), m_warnings(w) {}
        result(error_type&& e, const warning_list& w) : m_error(std::make_unique<error_type>(e)), m_warnings(w) {}
        result(const error_type& e, warning_list&& w) : m_error(std::make_unique<error_type>(e)), m_warnings(w) {}
        result(const error_type& e, const warning_list& w) : m_error(std::make_unique<error_type>(e)), m_warnings(w) {}

        operator bool() const noexcept { return !m_error.get(); }
        bool operator ==(const valid_t&) const noexcept { return !m_error.get(); }
        bool operator !=(const valid_t&) const noexcept { return m_error.get(); }
        bool valid() const noexcept { return !m_error.get(); }
        bool has_error() const noexcept { return m_error.get(); }

        const error_type& error() const
        {
            if (m_error) return *m_error;
            throw std::runtime_error("this result would have been valid!");
        }

        error_type&& error()
        {
            if (m_error) return std::move(*m_error);
            throw std::runtime_error("this result would have been valid!");
        }

        operator error_type() { return error(); }
        operator error_type() const { return error(); }

        bool has_warnings() const noexcept { return !m_warnings.empty(); }
        size_t warning_count() const noexcept { return m_warnings.size(); }
        const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        Warning& warning_at(size_t i) { return m_warnings[i]; }
        const warning_list& warnings() const { return m_warnings; }
        warning_list& warnings() { return m_warnings; }


        template<typename Func, typename... Args>
        void except(Func&& f, Args&&... args)
        {
            if (m_error.get())
                std::invoke(std::forward<Func>(f), *m_error, std::forward<Args>(args)...);
        }

    private:
        std::unique_ptr<error_type> m_error;
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

        result(warning_list&& w) : m_warnings(w) {}
        result(const warning_list& w) : m_warnings(w) {}

        result(success_type&& s) : m_success(std::make_unique<success_type>(s)) {}
        result(const success_type& s) : m_success(std::make_unique<success_type>(s)) {}
        result(success_type&& s, warning_list&& w) : m_success(std::make_unique<success_type>(s)), m_warnings(w) {}
        result(success_type&& s, const warning_list& w) : m_success(std::make_unique<success_type>(s)), m_warnings(w) {}
        result(const success_type& s, warning_list&& w) : m_success(std::make_unique<success_type>(s)), m_warnings(w) {}
        result(const success_type& s, const warning_list& w) : m_success(std::make_unique<success_type>(s)), m_warnings(w) {}

        operator bool() const noexcept { return m_success.get(); }
        bool operator ==(const valid_t&) const noexcept { return m_success.get(); }
        bool operator !=(const valid_t&) const noexcept { return !m_success.get(); }
        bool valid() const noexcept { return m_success.get(); }
        bool has_error() const noexcept { return !m_success.get(); }

        const success_type& value() const
        {
            if (m_success) return *m_success;
            throw legion_exception_msg("this result is invalid!");
        }

        success_type&& value()
        {
            if (m_success) return std::move(*m_success);
            throw legion_exception_msg("this result is invalid!");
        }

        operator success_type() { return value(); }
        operator success_type() const { return value(); }

        bool has_warnings() const noexcept { return !m_warnings.empty(); }
        size_t warning_count() const noexcept { return m_warnings.size(); }
        const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        Warning& warning_at(size_t i) { return m_warnings[i]; }
        const warning_list& warnings() const { return m_warnings; }
        warning_list& warnings() { return m_warnings; }


        template<typename Func, typename... Args>
        auto except(Func&& f, Args&&... args) -> decltype(std::invoke(std::declval<Func>(), std::declval<Args>()...))
        {
            if (!m_success)
                return std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
            return *m_success;
        }

    private:
        std::unique_ptr<success_type> m_success;
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

        result(warning_list&& w) : m_warnings(w) {}
        result(const warning_list& w) : m_warnings(w) {}

        operator bool() const noexcept { return m_succeeded; }
        bool operator ==(const valid_t&) const noexcept { return m_succeeded; }
        bool operator !=(const valid_t&) const noexcept { return !m_succeeded; }
        bool valid() const noexcept { return m_succeeded; }
        bool has_error() const noexcept { return !m_succeeded; }

        bool has_warnings() const noexcept { return !m_warnings.empty(); }
        size_t warning_count() const noexcept { return m_warnings.size(); }
        const Warning& warning_at(size_t i) const { return m_warnings[i]; }
        Warning& warning_at(size_t i) { return m_warnings[i]; }
        const warning_list& warnings() const { return m_warnings; }
        warning_list& warnings() { return m_warnings; }


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

}
