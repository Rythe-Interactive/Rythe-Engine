#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <core/platform/platform.hpp>

/**
 * @file sparse_map.hpp
 */

namespace args::core
{
	/**@class sparse_map
	 * @brief Quick lookup contiguous map. The map is based on the concept of a sparse set and thus inherits it's lookup complexity and contiguous nature.
	 * @tparam key_type The type to be used as the key.
	 * @tparam value_type The type to be used as the value.
	 * @tparam dense_type Container to be used to store the values.
	 * @tparam sparse_type Container to be used to store the keys.
	 * @note With default container parameters iterators may be invalidated upon resize. See reference of std::vector.
	 * @note Removing item might invalidate the itterator of the last item in the dense container.
	 */
	template <typename key_type, typename value_type, template<typename...> typename dense_type = std::vector, template<typename...> typename sparse_type = std::unordered_map>
	class sparse_map
	{
	public:
		using self_type = sparse_map<key_type, value_type, dense_type, sparse_type>;
		using self_reference = self_type&;
		using self_const_reference = const self_type&;

		using size_type = std::size_t;

		using key_reference = key_type&;
		using key_const_reference = const key_type&;
		using key_pointer = key_type*;

		using value_reference = value_type&;
		using value_const_reference = const value_type&;
		using value_pointer = value_type*;

		using sparse_container = sparse_type<key_type, size_type>;
		using dense_container = dense_type<std::pair<key_type, value_type>>;

		using iterator = typename dense_container::iterator;
		using const_iterator = typename dense_container::const_iterator;

	private:
		dense_container m_dense;
		sparse_container m_sparse;

		size_type m_size = 0;
		size_type m_capacity = 0;

	public:
		A_NODISCARD iterator begin() { return m_dense.begin(); }
		A_NODISCARD const_iterator begin() const { return m_dense.cbegin(); }

		A_NODISCARD iterator end() { return m_dense.begin() + m_size; }
		A_NODISCARD const_iterator end() const { return m_dense.cbegin() + m_size; }

		/**@brief Returns the amount of items in the sparse_map.
		 * @returns size_type Current amount of items contained in sparse_map.
		 */
		A_NODISCARD size_type size() const noexcept { return m_size; }

		/**@brief Returns the capacity of items the sparse_map could at least store without invalidating the iterators.
		 * @returns size_type Current capacity of the dense container.
		 */
		A_NODISCARD size_type capacity() const noexcept { return m_capacity; }

		/**@brief Returns whether the sparse_map is empty.
		 * @returns bool True if the sparse_map is empty, otherwise false.
		 */
		A_NODISCARD bool empty() const noexcept { return m_size == 0; }

		/**@brief Clears sparse_map.
		 * @note Will not update capacity.
		 */
		void clear() noexcept { m_size = 0; }

		/**@brief Reserves space in dense container for more items.
		 * @param size Amount of items to reserve space for (would be the new capacity).
		 * @note Will update capacity if resize happened.
		 */
		void reserve(size_type size)
		{
			if (size > m_capacity)
			{
				m_dense.resize(size);
				m_capacity = size;
			}
		}

#pragma region count
		/**@brief Returns the amount of items linked to a certain key.
		 * @param key Key to look for.
		 * @returns size_type Amount of items linked to the key (either 0 or 1).
		 * @note Function is only available for compatibility reasons, it is adviced to use contains instead.
		 * @ref args::core::sparse_map::contains
		 */
		A_NODISCARD size_type count(key_const_reference key) const
		{
			return contains(key);
		}

		/**@brief Returns the amount of items linked to a certain key.
		 * @param key Key to look for.
		 * @returns size_type Amount of items linked to the key (either 0 or 1).
		 * @note Function is only available for compatibility reasons, it is adviced to use contains instead.
		 * @ref args::core::sparse_map::contains
		 */
		A_NODISCARD size_type count(key_type&& key) const
		{
			return contains(key);
		}
#pragma endregion

#pragma region contains
		/**@brief Checks whether a certain key is contained in the sparse_map.
		 * @param key Key to check for.
		 * @returns bool true if the key was found, otherwise false.
		 */
		A_NODISCARD bool contains(key_const_reference key)
		{
			return m_sparse[key] >= 0 && m_sparse[key] < m_size && m_dense[m_sparse[key]].first == key;
		}

		/**@brief Checks whether a certain key is contained in the sparse_map.
		 * @param key Key to check for.
		 * @returns bool true if the key was found, otherwise false.
		 */
		A_NODISCARD bool contains(key_type&& key)
		{
			return m_sparse[key] >= 0 && m_sparse[key] < m_size && m_dense[m_sparse[key]].first == key;
		}

		/**@brief Checks whether a certain key is contained in the sparse_map.
		 * @param key Key to check for.
		 * @returns bool true if the key was found, otherwise false.
		 */
		A_NODISCARD bool contains(key_const_reference key) const
		{
			return m_sparse.at(key) >= 0 && m_sparse.at(key) < m_size && m_dense[m_sparse.at(key)].first == key;
		}

		/**@brief Checks whether a certain key is contained in the sparse_map.
		 * @param key Key to check for.
		 * @returns bool true if the key was found, otherwise false.
		 */
		A_NODISCARD bool contains(key_type&& key) const
		{
			return m_sparse.at(key) >= 0 && m_sparse.at(key) < m_size && m_dense[m_sparse.at(key)].first == key;
		}
#pragma endregion

#pragma region find
		/**@brief Finds the iterator of a value using std::find.
		 * @param val Value to find.
		 * @returns Iterator to the value if found, otherwise end.
		 */
		A_NODISCARD iterator find(value_const_reference val)
		{
			return std::find(begin(), end(), val);
		}

		/**@brief Finds the iterator of a value using std::find.
		 * @param val Value to find.
		 * @returns Iterator to the value if found, otherwise end.
		 */
		A_NODISCARD const_iterator find(value_const_reference val) const
		{
			return std::find(begin(), end(), val);
		}
#pragma endregion

#pragma region insert
		/**@brief Inserts new item into sparse_map
		 * @param key Key to insert the new item to.
		 * @param val Value to insert and link to the key.
		 * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
		 */
		std::pair<iterator, bool> insert(key_const_reference key, value_const_reference val)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, val };
				m_sparse[key] = m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}

		/**@brief Inserts new item into sparse_map
		 * @param key Key to insert the new item to.
		 * @param val Value to insert and link to the key.
		 * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
		 */
		std::pair<iterator, bool> insert(key_type&& key, value_const_reference val)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, val };
				m_sparse[key] = m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}

		/**@brief Inserts new item into sparse_map
		 * @param key Key to insert the new item to.
		 * @param val Value to insert and link to the key.
		 * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
		 */
		std::pair<iterator, bool> insert(key_const_reference key, value_type&& val)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, val };
				m_sparse[key] = m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}

		/**@brief Inserts new item into sparse_map
		 * @param key Key to insert the new item to.
		 * @param val Value to insert and link to the key.
		 * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
		 */
		std::pair<iterator, bool> insert(key_type&& key, value_type&& val)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, val };
				m_sparse[key] = m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}
#pragma endregion

#pragma region emplace
		/**@brief Construct item in place.
		 * @param key Key to which the item should be created.
		 * @param arguments Arguments to pass to the item constructor.
		 */
		template<typename... Arguments>
		std::pair<iterator, bool> emplace(key_const_reference key, Arguments&&... arguments)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, value_type(arguments...) };
				m_sparse[key] = m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}

		/**@brief Construct item in place.
		 * @param key Key to which the item should be created.
		 * @param arguments Arguments to pass to the item constructor.
		 */
		template<typename... Arguments>
		std::pair<iterator, bool> emplace(key_type&& key, Arguments&&... arguments)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, value_type(arguments...) };
				m_sparse[key] = m_size;
				++m_size;
				return std::make_pair(itr, true);
			}
			return std::make_pair(m_dense.end(), false);
		}
#pragma endregion

#pragma region operator[]
		/**@brief Returns item from sparse_map, inserts default value if it doesn't exist yet.
		 * @param key Key value that needs to be retrieved.
		 */
		value_reference operator[](key_type&& key)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, value_type() };
				m_sparse[key] = m_size;
				++m_size;
			}

			return m_dense[m_sparse[key]].second;
		}

		/**@brief Returns item from sparse_map, inserts default value if it doesn't exist yet.
		 * @param key Key value that needs to be retrieved.
		 */
		value_reference operator[](key_const_reference key)
		{
			if (!contains(key))
			{
				if (m_size >= m_capacity)
					reserve(m_size + 1);

				auto itr = m_dense.begin() + m_size;
				*itr = { key, value_type() };
				m_sparse[key] = m_size;
				++m_size;
			}

			return m_dense[m_sparse[key]].second;
		}

		/**@brief Returns const item from const sparse_map.
		 * @param key Key value that needs to be retrieved.
		 */
		value_const_reference operator[](key_type&& key) const
		{
			if (!contains(key))
				throw std::out_of_range("Sparse map does not contain this key and is non modifiable.");

			return m_dense[m_sparse.at(key)].second;
		}

		/**@brief Returns const item from const sparse_map.
		 * @param key Key value that needs to be retrieved.
		 */
		value_const_reference operator[](key_const_reference key) const
		{
			if (!contains(key))
				throw std::out_of_range("Sparse map does not contain this key and is non modifiable.");

			return m_dense[m_sparse.at(key)].second;
		}
#pragma endregion

		/**@brief Erases item from sparse_map.
		 * @param key Key value that needs to be erased.
		 */
		size_type erase(key_const_reference key)
		{
			if (contains(key))
			{
				m_dense[m_sparse[key]] = m_dense[m_size - 1];
				m_sparse[m_dense[m_size - 1].first] = m_sparse[key];
				--m_size;
				return true;
			}
			return false;
		}
	};
}