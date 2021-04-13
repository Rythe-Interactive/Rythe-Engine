#pragma once
#include <unordered_map>
#include <memory>
#include <type_traits>

#include <core/common/assert.hpp>
#include <core/containers/delegate.hpp>
#include <core/async/rw_spinlock.hpp>

namespace legion::core::common
{
    namespace detail
    {
        template<typename T>
        struct _managed_resource_del
        {
        private:
#if defined(LEGION_DEBUG)
            T& m_store;
#endif
            delegate<void(T&)>& m_destroyFunc;

        public:
            _managed_resource_del(_managed_resource_del&&) noexcept = default;

#if defined(LEGION_DEBUG)
            constexpr _managed_resource_del(T& src, delegate<void(T&)>& destroyFunc) noexcept : m_store(src), m_destroyFunc(destroyFunc) {}
#else
            constexpr _managed_resource_del(delegate<void(T&)> destroyFunc) noexcept : m_destroyFunc(destroyFunc) {}
#endif

            void operator()(T* const _ptr) const
            {
#if defined(LEGION_DEBUG)
                assert_msg("Managed resource deleter is a mock deleter and should not be used for dynamic memory management.", _ptr == &m_store);
#endif
                    m_destroyFunc(*_ptr);
            }
        };
    }

    template<typename T>
    struct managed_resource
    {
    public:
        T value;

    private:
        delegate<void(T&)> m_destroyFunc;
        std::shared_ptr<T> m_ref_counter;
    public:
        explicit managed_resource(std::nullptr_t) : value(), m_destroyFunc(nullptr), m_ref_counter(nullptr) {}

        template<typename... Args>
        managed_resource(delegate<void(T&)> destroyFunc, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
            : value(std::forward<Args>(args)...), m_destroyFunc(destroyFunc),
#if defined(LEGION_DEBUG)
            m_ref_counter(&value, detail::_managed_resource_del<T>{value, m_destroyFunc})
#else
            m_ref_counter(&value, detail::_managed_resource_del<T>{ m_destroyFunc })
#endif
        {
        }

        managed_resource(const managed_resource<T>& src) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : value(src.value), m_destroyFunc(src.m_destroyFunc), m_ref_counter(src.m_ref_counter)
        {
        }

        managed_resource(managed_resource<T>&& src) noexcept(std::is_nothrow_move_constructible_v<T>)
            : value(std::move(src.value)), m_destroyFunc(std::move(src.m_destroyFunc)), m_ref_counter(std::move(src.m_ref_counter))
        {
        }

        managed_resource<T>& operator=(const managed_resource<T>& src) noexcept(std::is_nothrow_copy_assignable_v<T>)
        {
            value = src.value;
            m_destroyFunc = src.m_destroyFunc;
            m_ref_counter = src.m_ref_counter;
            return *this;
        }

        managed_resource<T>& operator=(managed_resource<T>&& src) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            value = std::move(src.value);
            m_destroyFunc = std::move(src.m_destroyFunc);
            m_ref_counter = std::move(src.m_ref_counter);
            return *this;
        }

        T* operator&()
        {
            return &value;
        }

        operator T& ()
        {
            return value;
        }

        operator const T& () const
        {
            return value;
        }
    };
}
