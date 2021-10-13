#pragma once
#include <utility>
#include <functional>
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>

namespace legion::core
{
    template <class T>
    struct pair_range
    {
        using iterator = T;
        pair_range(const std::pair<T, T> r) noexcept : range(r)
        {
        }

        template<typename ItType>
        pair_range(ItType begin, ItType end) noexcept : range(std::move(begin), std::move(end))
        {
        }

        L_NODISCARD auto& begin() const
        {
            return range.first;
        }

        L_NODISCARD auto& end() const
        {
            return range.second;
        }
        std::pair<T, T> range;
    };


    #if !defined(DOXY_EXCLUDE)
    template <class T>
    pair_range(std::pair<T, T>)->pair_range<T>;

    template <class T>
    pair_range(T begin, T end)->pair_range<std::remove_reference_t<T>>;
    #endif

    template <class It>
    bool checked_next(It& iter, It end, std::size_t diff)
    {
        while (diff-- > 0)
        {
            if (iter == end) return false;
            ++iter;
        }
        return true;
    }

    template <class KeysIterator, class ValuesIterator, class Category = std::bidirectional_iterator_tag, class Diff = diff_type>
    class key_value_pair_iterator
    {
    public:
        using keys_proxy_type = KeysIterator;
        using values_proxy_type = ValuesIterator;
        using key_type = typename keys_proxy_type::value_type;
        using value_type = typename values_proxy_type::value_type;
        using pair_type = std::pair<key_type&, value_type&>;
        using const_pair_type = std::pair<const key_type&, const value_type&>;

        using iterator_category = Category;
        using difference_type = Diff;

        explicit key_value_pair_iterator(keys_proxy_type keys, values_proxy_type values) : m_key(keys), m_value(values) {}

        pair_type operator*()
        {
            return { std::ref(key()), std::ref(value()) };
        }
        const_pair_type operator*() const
        {
            return { std::cref(key()), std::cref(value()) };
        }

        pair_type operator->()
        {
            return { std::ref(key()), std::ref(value()) };
        }
        const_pair_type operator->() const
        {
            return { std::cref(key()), std::cref(value()) };
        }

        auto& key()
        {
            return *m_key;
        }
        const auto& key() const
        {
            return *m_key;
        }
        auto& value()
        {
            return *m_value;
        }
        const auto& value() const
        {
            return *m_value;
        }

        friend auto operator==(const key_value_pair_iterator& rhs, const key_value_pair_iterator& lhs)
        {
            return rhs.m_key == lhs.m_key && rhs.m_value == lhs.m_value;
        }
        friend auto operator!=(const key_value_pair_iterator& rhs, const key_value_pair_iterator& lhs)
        {
            return rhs.m_key != lhs.m_key || rhs.m_value != lhs.m_value;
        }

        auto& operator++() noexcept
        {
            m_key++;
            m_value++;
            return *this;
        }

        auto operator++(int) noexcept
        {
            return key_value_pair_iterator(++m_key, ++m_value);
        }

        auto& operator--() noexcept
        {
            m_key--;
            m_value--;
            return *this;
        }

        auto operator--(int) noexcept
        {
            return key_value_pair_iterator(--m_key, --m_value);
        }

    private:
        keys_proxy_type m_key;
        values_proxy_type m_value;
    };

    template <class PairIteratorContainer>
    class key_only_iterator
    {
    public:
        using self_proxy_type = typename PairIteratorContainer::iterator;
        explicit key_only_iterator(self_proxy_type self) : m_self(self) {}

        auto& operator*()
        {
            return key();
        }
        const auto& operator*() const
        {
            return key();
        }

        auto& operator->()
        {
            return key();
        }
        const auto& operator->() const
        {
            return key();
        }
        auto& key()
        {
            return m_self->first;
        }
        const auto& key() const
        {
            return m_self->first;
        }
        friend auto operator==(const key_only_iterator& rhs, const key_only_iterator& lhs)
        {
            return rhs.m_self == lhs.m_self;
        }
        friend auto operator!=(const key_only_iterator& rhs, const key_only_iterator& lhs)
        {
            return rhs.m_self != lhs.m_self;
        }

        auto operator++()
        {
            return key_only_iterator(m_self++);
        }
        auto operator++() const
        {
            return key_only_iterator(m_self++);
        }

    private:
        self_proxy_type m_self;
    };

    template <class PairIteratorContainer>
    class value_only_iterator
    {
    public:
        using self_proxy_type = typename PairIteratorContainer::iterator;
        explicit value_only_iterator(self_proxy_type self) : m_self(std::move(self)) {}

        auto& operator*()
        {
            return value();
        }
        const auto& operator*() const
        {
            return value();
        }

        auto& operator->()
        {
            return value();
        }
        const auto& operator->() const
        {
            return value();
        }
        auto& value()
        {
            return m_self->second;
        }
        const auto& value() const
        {
            return m_self->second;
        }
        auto operator++()
        {
            return value_only_iterator(m_self++);
        }
        auto operator++() const
        {
            return value_only_iterator(m_self++);
        }
        friend auto operator==(const value_only_iterator& rhs, const value_only_iterator& lhs)
        {
            return rhs.m_self == lhs.m_self;
        }
        friend auto operator!=(const value_only_iterator& rhs, const value_only_iterator& lhs)
        {
            return rhs.m_self != lhs.m_self;
        }
    private:
        self_proxy_type m_self;
    };

    template <class PairIteratorContainer>
    class keys_only
    {
    public:

        explicit keys_only(PairIteratorContainer& cont) : m_container(cont) {}
        L_NODISCARD auto begin() const
        {
            return key_only_iterator<PairIteratorContainer>(m_container.begin());
        }
        L_NODISCARD auto end() const
        {
            return key_only_iterator<PairIteratorContainer>(m_container.end());
        }
    private:
        PairIteratorContainer& m_container;
    };

#if !defined(DOXY_EXCLUDE)
    template <class PairIteratorContainer>
    keys_only(PairIteratorContainer&) -> keys_only<PairIteratorContainer>;
#endif

    template <class PairIteratorContainer>
    class values_only
    {
    public:

        explicit values_only(PairIteratorContainer& cont) : m_container(cont) {}

        L_NODISCARD auto begin() const
        {
            return value_only_iterator<PairIteratorContainer>(m_container.begin());
        }
        L_NODISCARD auto end() const
        {
            return value_only_iterator<PairIteratorContainer>(m_container.end());
        }
    private:
        PairIteratorContainer& m_container;
    };

#if !defined(DOXY_EXCLUDE)
    template <class PairIteratorContainer>
    values_only(PairIteratorContainer&) -> values_only<PairIteratorContainer>;
#endif

}
