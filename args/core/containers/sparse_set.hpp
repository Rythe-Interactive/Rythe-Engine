#pragma once
#include <vector>
#include <type_traits>
#include <algorithm>
#include <core/platform/platform.hpp>

/**
 * @file sparse_set.hpp
 */


namespace args::core
{
	template <typename value_type, template<typename> typename dense_type = std::vector, template<typename> typename sparse_type = std::vector>
	class sparse_set
	{
		static_assert(std::is_unsigned_v<value_type>, "value_type must an unsigned type.");
	public:
		using sparse_container = sparse_type<value_type>;
		using dense_container = dense_type<value_type>;

		using size_type = std::size_t;

		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = typename dense_container::iterator;
		using const_iterator = typename dense_container::const_iterator;

		A_NODISCARD iterator begin() { return m_dense.begin(); }
		A_NODISCARD const_iterator begin() const { return m_dense.cbegin(); }

		A_NODISCARD iterator end() { return m_dense.begin() + m_size; }
		A_NODISCARD const_iterator end() const { return m_dense.cbegin() + m_size; }

		A_NODISCARD size_type size() const noexcept { return m_size; }

		A_NODISCARD size_type capacity() const noexcept { return m_capacity; }

		A_NODISCARD bool empty() const noexcept { return m_size == 0; }

		void clear() noexcept { m_size = 0; }
		void reserve(size_type size)
		{
			if (size > m_capacity)
			{
				m_dense.resize(size, 0);
				m_sparse.resize(size, 0);
				m_capacity = size;
			}
		}

		size_type count(const_reference val) const
		{
			return contains(val);
		}
		size_type count(value_type&& val) const
		{
			return contains(val);
		}

		bool contains(const_reference val) const
		{
			return val < m_capacity&&
				m_sparse[val] < m_size&&
				m_dense[m_sparse[val]] == val;
		}
		bool contains(value_type&& val) const
		{
			return val < m_capacity&&
				m_sparse[val] < m_size&&
				m_dense[m_sparse[val]] == val;
		}

		bool contains(const sparse_set<value_type>& other) const
		{
			if (other.m_size == 0)
				return true;

			if (m_size == 0 || m_size < other.m_size)
				return false;

			bool overlap = true;
			for (const_reference item : other)
				overlap = overlap && (item < m_capacity&& m_sparse[item] < m_size&& m_dense[m_sparse[item]] == item);

			return overlap;
		}

		iterator find(const_reference val)
		{
			return std::find(m_dense.begin(), m_dense.end(), val);
		}
		const_iterator find(const_reference val) const
		{
			return std::find(m_dense.begin(), m_dense.end(), val);
		}

		std::pair<iterator, bool> insert(const_reference val)
		{
			if (!contains(val))
			{
				if (val >= m_capacity)
					reserve(val + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = val;
				m_sparse[val] = (value_type)m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}
		std::pair<iterator, bool> insert(value_type&& val)
		{
			if (!contains(val))
			{
				if (val >= m_capacity)
					reserve(val + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = val;
				m_sparse[val] = (value_type)m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}

		size_type erase(const_reference val)
		{
			if (contains(val))
			{
				m_dense[m_sparse[val]] = m_dense[m_size - 1];
				m_sparse[m_dense[m_size - 1]] = m_sparse[val];
				--m_size;
				return true;
			}
			return false;
		}
	private:
		dense_container m_dense;
		sparse_container m_sparse;

		size_type m_size = 0;
		size_type m_capacity = 0;
	};
}