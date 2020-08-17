#include <utility>


#pragma once

namespace args::core::iterator
{
    template <class T>
	struct pair_range
	{
        using iterator = T;
		pair_range(const std::pair<T,T> r) : range(r)
		{
		}

		[[nodiscard]] auto& begin() const
		{
			return range.first;
		}

		[[nodiscard]] auto& end() const
		{
			return range.second;
		}
		std::pair<T,T> range;
	};

    template <class It>
	bool checked_next(It& iter,It end, std::size_t diff)
	{
		while(diff --> 0 )
		{
			if(iter == end) return false;
			++iter;
		}
		return true;
	}

    template <class PairIteratorContainer>
    class key_only_iterator
    {
    public:
        using self_proxy_type = typename PairIteratorContainer::iterator;
        explicit key_only_iterator(self_proxy_type self) : m_self(self){}

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
        friend auto operator==(const key_only_iterator& rhs,const key_only_iterator& lhs)
        {
            return rhs.m_self == lhs.m_self;
        }
        friend auto operator!=(const key_only_iterator& rhs,const key_only_iterator& lhs)
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
        explicit value_only_iterator(self_proxy_type self) : m_self(std::move(self)){}

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
        friend auto operator==(const value_only_iterator& rhs,const value_only_iterator& lhs)
        {
            return rhs.m_self == lhs.m_self;
        }
        friend auto operator!=(const value_only_iterator& rhs,const value_only_iterator& lhs)
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

        explicit keys_only(PairIteratorContainer& cont) : m_container(cont){}
        A_NODISCARD auto begin() const
        {
            return key_only_iterator<PairIteratorContainer>(m_container.begin());
        }
        A_NODISCARD auto end() const
        {
            return key_only_iterator<PairIteratorContainer>(m_container.end());
        }
    private:
        PairIteratorContainer& m_container;
    };

    template <class PairIteratorContainer>
    class values_only
    {
    public:

        explicit values_only(PairIteratorContainer& cont) : m_container(cont){}

        A_NODISCARD auto begin() const
        {
            return value_only_iterator<PairIteratorContainer>(m_container.begin());
        }
        A_NODISCARD auto end() const
        {
            return value_only_iterator<PairIteratorContainer>(m_container.end());
        }
    private:
        PairIteratorContainer& m_container;
    };

}
