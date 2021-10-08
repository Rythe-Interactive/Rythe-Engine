#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>

/**
 * @file pointer.hpp
 */

namespace legion::core
{
    /**@class pointer
     * @brief Non owning pointer wrapper.
     * @tparam T Type of the underlying pointer.
     */
    template<typename T>
    struct pointer
    {
        T* ptr;

        RULE_OF_5_CONSTEXPR_NOEXCEPT(pointer);

        constexpr L_ALWAYS_INLINE pointer(T* other) noexcept : ptr(other) {}
        constexpr L_ALWAYS_INLINE pointer& operator=(T* other) noexcept { ptr = other; return *this; }

        L_NODISCARD constexpr L_ALWAYS_INLINE operator bool() const noexcept { return ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE operator T* () const noexcept { return ptr; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(const pointer& other) const noexcept { return ptr == other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(T* other) const noexcept { return ptr == other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(std::nullptr_t) const noexcept { return ptr == nullptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(const pointer& other) const noexcept { return ptr != other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(T* other) const noexcept { return ptr != other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(std::nullptr_t) const noexcept { return ptr != nullptr; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator< (const pointer& other) const noexcept { return ptr < other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator< (T* other) const noexcept { return ptr < other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator> (const pointer& other) const noexcept { return ptr > other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator> (T* other) const noexcept { return ptr > other; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator<=(const pointer& other) const noexcept { return ptr <= other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator<=(T* other) const noexcept { return ptr <= other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator>=(const pointer& other) const noexcept { return ptr >= other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator>=(T* other) const noexcept { return ptr >= other; }

        L_NODISCARD constexpr L_ALWAYS_INLINE T* operator->() const noexcept { return ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE T& operator*() const noexcept { return *ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE T& operator[](diff_type diff) const noexcept { return *(ptr + diff); }

        constexpr L_ALWAYS_INLINE pointer& operator++() noexcept { ptr++; return *this; }
        constexpr L_ALWAYS_INLINE pointer operator++(int) noexcept { return { ++ptr }; }

        constexpr L_ALWAYS_INLINE pointer& operator--() noexcept { ptr--; return *this; }
        constexpr L_ALWAYS_INLINE pointer operator--(int) noexcept { return { --ptr }; }

        constexpr L_ALWAYS_INLINE pointer& operator+=(pointer other) noexcept { ptr += other.ptr; return *this; }
        constexpr L_ALWAYS_INLINE pointer& operator+=(diff_type n) noexcept { ptr += n; return *this; }
        L_NODISCARD constexpr L_ALWAYS_INLINE pointer operator+(pointer other) const noexcept { return { ptr + other.ptr }; }
        L_NODISCARD constexpr L_ALWAYS_INLINE pointer operator+(diff_type n) const noexcept { return { ptr + n }; }

        constexpr L_ALWAYS_INLINE pointer& operator-=(pointer other) noexcept { ptr -= other.ptr; return *this; }
        constexpr L_ALWAYS_INLINE pointer& operator-=(diff_type n) noexcept { ptr -= n; return *this; }
        L_NODISCARD constexpr L_ALWAYS_INLINE pointer operator-(pointer other) const noexcept { return { ptr - other.ptr }; }
        L_NODISCARD constexpr L_ALWAYS_INLINE pointer operator-(diff_type n) const noexcept { return { ptr - n }; }
    };

    template<>
    struct pointer<void>
    {
        void* ptr;

        RULE_OF_5_NOEXCEPT(pointer);

        constexpr L_ALWAYS_INLINE pointer(void* other) noexcept : ptr(other) {}
        constexpr L_ALWAYS_INLINE pointer& operator=(void* other) noexcept { ptr = other; return *this; }

        L_NODISCARD constexpr L_ALWAYS_INLINE operator bool() const noexcept { return ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE operator void* () const noexcept { return ptr; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(const pointer& other) const noexcept { return ptr == other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(void* other) const noexcept { return ptr == other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(std::nullptr_t) const noexcept { return ptr == nullptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(const pointer& other) const noexcept { return ptr != other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(void* other) const noexcept { return ptr != other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(std::nullptr_t) const noexcept { return ptr != nullptr; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator< (const pointer& other) const noexcept { return ptr < other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator< (void* other) const noexcept { return ptr < other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator> (const pointer& other) const noexcept { return ptr > other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator> (void* other) const noexcept { return ptr > other; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator<=(const pointer& other) const noexcept { return ptr <= other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator<=(void* other) const noexcept { return ptr <= other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator>=(const pointer& other) const noexcept { return ptr >= other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator>=(void* other) const noexcept { return ptr >= other; }
    };

    template<>
    struct pointer<const void>
    {
        const void* ptr;

        RULE_OF_5_NOEXCEPT(pointer);

        constexpr L_ALWAYS_INLINE pointer(const void* other) noexcept : ptr(other) {}
        constexpr L_ALWAYS_INLINE pointer& operator=(const void* other) noexcept { ptr = other; return *this; }

        L_NODISCARD constexpr L_ALWAYS_INLINE operator bool() const noexcept { return ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE operator const void* () const noexcept { return ptr; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(const pointer& other) const noexcept { return ptr == other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(const void* other) const noexcept { return ptr == other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator==(std::nullptr_t) const noexcept { return ptr == nullptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(const pointer& other) const noexcept { return ptr != other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(const void* other) const noexcept { return ptr != other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator!=(std::nullptr_t) const noexcept { return ptr != nullptr; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator< (const pointer& other) const noexcept { return ptr < other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator< (const void* other) const noexcept { return ptr < other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator> (const pointer& other) const noexcept { return ptr > other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator> (const void* other) const noexcept { return ptr > other; }

        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator<=(const pointer& other) const noexcept { return ptr <= other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator<=(const void* other) const noexcept { return ptr <= other; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator>=(const pointer& other) const noexcept { return ptr >= other.ptr; }
        L_NODISCARD constexpr L_ALWAYS_INLINE bool operator>=(const void* other) const noexcept { return ptr >= other; }
    };
}
