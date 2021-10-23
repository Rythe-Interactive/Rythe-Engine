#pragma once
#include <new>
#include <iterator>
#include <type_traits>
#include <algorithm>

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>

namespace legion::core
{
    template<typename _Val>
    struct alloconly_allocator
    {
    public:
        static_assert(!std::is_const_v<_Val>, "The C++ Standard forbids containers of const elements because allocator<const T> is ill-formed.");

        using value_type = _Val;
        using size_type = ::legion::core::size_type;
        using difference_type = ::legion::core::diff_type;

        RULE_OF_5_CONSTEXPR_NOEXCEPT(alloconly_allocator);

        template<typename T>
        constexpr alloconly_allocator(const alloconly_allocator<T>& other) noexcept {}

        L_NODISCARD inline L_ALWAYS_INLINE value_type* allocate(const size_type count)
        {
            if (count == 0)
                return nullptr;

            return static_cast<value_type*>(allocate_bytes(sizeof(value_type) * count));
        }

        inline L_ALWAYS_INLINE void deallocate(value_type* const ptr, const size_type count)
        {
            deallocate_bytes(ptr, sizeof(value_type) * count);
        }


        template<typename T, typename... Args>
        inline L_ALWAYS_INLINE void construct(T*, Args&&...) {}

        template<typename T>
        inline L_ALWAYS_INLINE void destroy(T*) {}

    private:
        constexpr static size_type m_stdCppDefaultNewAlignment = static_cast<size_type>(16ull);
        constexpr static size_type m_alignment = std::max(alignof(value_type), m_stdCppDefaultNewAlignment);
        constexpr static size_type m_bigAllocationThreshold = static_cast<size_type>(4096ull);
        constexpr static size_type m_bigAllocationAlignment = std::max(m_alignment, static_cast<size_type>(32ull));

        inline L_ALWAYS_INLINE size_type alloc_alignment(size_type alloc)
        {
            return (alloc > m_bigAllocationThreshold ? m_bigAllocationAlignment : m_alignment);
        }

        inline L_ALWAYS_INLINE void* allocate_bytes(const size_type count)
        {
            return (count == 0 ? nullptr : ::operator new (count, std::align_val_t{ alloc_alignment(count) }));
        }

        inline L_ALWAYS_INLINE void deallocate_bytes(void* ptr, const size_type count) noexcept
        {
            if (count == 0)
                return;
            ::operator delete (ptr, count, std::align_val_t{ alloc_alignment(count) });
        }
    };
}
