#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <stdexcept>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/iterator_tricks.hpp>
#include <core/common/alloconly_allocator.hpp>

/**
 * @file sparse_map.hpp
 */

namespace legion::core
{
    /**@class sparse_map
     * @brief Quick lookup contiguous map. The map is based on the concept of a sparse set and thus inherits it's lookup complexity and contiguous nature.
     * @tparam key_type The type to be used as the key.
     * @tparam value_type The type to be used as the value.
     * @tparam dense_type Container to be used to store the values.
     * @tparam sparse_type Container to be used to store the keys.
     * @note With default container parameters iterators may be invalidated upon resize. See reference of std::vector.
     * @note Removing item might invalidate the iterator of the last item in the dense container.
     */
    template <typename _Key, typename _Val, typename _Hash = std::hash<_Key>, template<typename...> typename _Dense = std::vector, template<typename...> typename _Sparse = std::unordered_map>
    class sparse_map
    {
    public:
        using self_type = sparse_map<_Key, _Val, _Hash, _Dense, _Sparse>;
        using self_reference = self_type&;
        using self_const_reference = const self_type&;

        using key_type = _Key;
        using key_reference = key_type&;
        using key_const_reference = const key_type&;
        using key_pointer = key_type*;

        using value_type = _Val;
        using value_reference = value_type&;
        using value_const_reference = const value_type&;
        using value_pointer = value_type*;

        using hash_type = _Hash;

        using sparse_container = _Sparse<key_type, size_type, hash_type>;
        using dense_value_container = _Dense<value_type, alloconly_allocator<value_type>>;
        using dense_key_container = _Dense<key_type, alloconly_allocator<key_type>>;

        using dense_value_iterator = typename dense_value_container::iterator;
        using dense_value_const_iterator = typename dense_value_container::const_iterator;
        using dense_value_reverse_iterator = typename dense_value_container::reverse_iterator;
        using dense_value_const_reverse_iterator = typename dense_value_container::const_reverse_iterator;
        using dense_value_range = pair_range<dense_value_iterator>;
        using dense_value_const_range = pair_range<dense_value_const_iterator>;
        using dense_value_reverse_range = pair_range<dense_value_reverse_iterator>;
        using dense_value_const_reverse_range = pair_range<dense_value_const_reverse_iterator>;

        using dense_key_iterator = typename dense_key_container::iterator;
        using dense_key_const_iterator = typename dense_key_container::const_iterator;
        using dense_key_reverse_iterator = typename dense_key_container::reverse_iterator;
        using dense_key_const_reverse_iterator = typename dense_key_container::const_reverse_iterator;
        using dense_key_range = pair_range<dense_key_iterator>;
        using dense_key_const_range = pair_range<dense_key_const_iterator>;
        using dense_key_reverse_range = pair_range<dense_key_reverse_iterator>;
        using dense_key_const_reverse_range = pair_range<dense_key_const_reverse_iterator>;

        using iterator = core::key_value_pair_iterator<typename dense_key_container::iterator, typename dense_value_container::iterator>;
        using const_iterator = core::key_value_pair_iterator<typename dense_key_container::const_iterator, typename dense_value_container::const_iterator>;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_itr_range = pair_range<reverse_iterator>;
        using const_reverse_itr_range = pair_range<const_reverse_iterator>;

    private:
        dense_value_container m_dense_value;
        dense_key_container m_dense_key;
        sparse_container m_sparse;

        size_type m_size = 0;
        size_type m_capacity = 0;

    public:
        NO_DEF_CTOR_RULE5_NOEXCEPT(sparse_map);

        sparse_map() { reserve(static_cast<size_type>(1ull)); }
        sparse_map(size_type capacity) { reserve(capacity); }



        L_NODISCARD dense_value_range values() noexcept { return pair_range{ m_dense_value.begin(), m_dense_value.begin() + m_size }; }
        L_NODISCARD dense_value_const_range values() const noexcept { return pair_range{ m_dense_value.cbegin(), m_dense_value.cbegin() + m_size }; }
        L_NODISCARD dense_value_const_range cvalues() const noexcept { return pair_range{ m_dense_value.cbegin(), m_dense_value.cbegin() + m_size }; }

        L_NODISCARD dense_key_range keys() noexcept { return pair_range{ m_dense_key.begin(), m_dense_key.begin() + m_size }; }
        L_NODISCARD dense_key_const_range keys() const noexcept { return pair_range{ m_dense_key.cbegin(), m_dense_key.cbegin() + m_size }; }
        L_NODISCARD dense_key_const_range ckeys() const noexcept { return pair_range{ m_dense_key.cbegin(), m_dense_key.cbegin() + m_size }; }

        L_NODISCARD iterator begin() noexcept { return iterator(m_dense_key.begin(), m_dense_value.begin()); }
        L_NODISCARD const_iterator begin() const noexcept { return const_iterator(m_dense_key.cbegin(), m_dense_value.cbegin()); }
        L_NODISCARD const_iterator cbegin() const noexcept { return const_iterator(m_dense_key.cbegin(), m_dense_value.cbegin()); }

        L_NODISCARD iterator end() noexcept { return iterator(m_dense_key.begin() + m_size, m_dense_value.begin() + m_size); }
        L_NODISCARD const_iterator end() const noexcept { return const_iterator(m_dense_key.cbegin() + m_size, m_dense_value.cbegin() + m_size); }
        L_NODISCARD const_iterator cend() const noexcept { return const_iterator(m_dense_key.cbegin() + m_size, m_dense_value.cbegin() + m_size); }

        L_NODISCARD dense_value_reverse_range rvalues() noexcept { return pair_range{ m_dense_value.rbegin() + (m_dense.size() - m_size), m_dense_value.rend() }; }
        L_NODISCARD dense_value_const_reverse_range rvalues() const noexcept { return pair_range{ m_dense_value.crbegin() + (m_dense.size() - m_size), m_dense_value.crend() }; }
        L_NODISCARD dense_value_const_reverse_range crvalues() const noexcept { return pair_range{ m_dense_value.crbegin() + (m_dense.size() - m_size), m_dense_value.crend() }; }

        L_NODISCARD dense_key_reverse_range rkeys() noexcept { return pair_range{ m_dense_key.rbegin() + (m_dense.size() - m_size), m_dense_key.rend() }; }
        L_NODISCARD dense_key_const_reverse_range rkeys() const noexcept { return pair_range{ m_dense_key.crbegin() + (m_dense.size() - m_size), m_dense_key.crend() }; }
        L_NODISCARD dense_key_const_reverse_range crkeys() const noexcept { return pair_range{ m_dense_key.crbegin() + (m_dense.size() - m_size), m_dense_key.crend() }; }

        L_NODISCARD reverse_iterator rbegin() noexcept { return reverse_iterator{ end() }; }
        L_NODISCARD const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{ cend() }; }
        L_NODISCARD const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{ cend() }; }

        L_NODISCARD reverse_iterator rend() noexcept { return reverse_iterator{ begin() }; }
        L_NODISCARD const_reverse_iterator rend() const noexcept { return reverse_iterator{ cbegin() }; }
        L_NODISCARD const_reverse_iterator crend() const noexcept { return reverse_iterator{ cbegin() }; }

        L_NODISCARD inline L_ALWAYS_INLINE reverse_itr_range reverse_range() noexcept { return pair_range{ rbegin(), rend() }; }
        L_NODISCARD inline L_ALWAYS_INLINE const_reverse_itr_range reverse_range() const noexcept { return pair_range{ crbegin(), crend() }; }
        L_NODISCARD inline L_ALWAYS_INLINE const_reverse_itr_range creverse_range() const noexcept { return pair_range{ crbegin(), crend() }; }

        /**@brief Returns the amount of items in the sparse_map.
         * @returns size_type Current amount of items contained in sparse_map.
         */
        L_NODISCARD size_type size() const noexcept { return m_size; }

        /**@brief Returns the capacity of items the sparse_map could at least store without invalidating the iterators.
         * @returns size_type Current capacity of the dense container.
         */
        L_NODISCARD size_type capacity() const noexcept { return m_capacity; }

        /**@brief Returns the maximum number of items the sparse_map could at most store without crashing.
         * @note This value typically reflects the theoretical limit on the size of the container, at most std::numeric_limits<difference_type>::max().
         *       At runtime, the size of the container may be limited to a value smaller than max_size() by the amount of RAM available.
         * @returns size_type
         */
        L_NODISCARD size_type max_size() const noexcept { return m_dense_value.max_size(); }

        /**@brief Returns whether the sparse_map is empty.
         * @returns bool True if the sparse_map is empty, otherwise false.
         */
        L_NODISCARD bool empty() const noexcept { return m_size == 0; }

        /**@brief Clears sparse_map.
         * @note Will not update capacity.
         */
        void clear() noexcept
        {
            m_size = 0;
            m_dense_value.clear();
            m_dense_key.clear();
            m_sparse.clear();
        }

        /**@brief Reserves space in dense container for more items.
         * @param size Amount of items to reserve space for (would be the new capacity).
         * @note Will update capacity if resize happened.
         */
        void reserve(size_type size)
        {
            if (size > m_capacity)
            {
                m_dense_value.reserve(size);
                m_dense_key.reserve(size);
                m_sparse.reserve(size);
                m_capacity = size;
            }
        }

#pragma region count
        /**@brief Returns the amount of items linked to a certain key.
         * @param key Key to look for.
         * @returns size_type Amount of items linked to the key (either 0 or 1).
         * @note Function is only available for compatibility reasons, it is advised to use contains instead.
         * @ref legion::core::sparse_map::contains
         */
        L_NODISCARD size_type count(key_const_reference key) const
        {
            return contains(key);
        }
#pragma endregion

#pragma region contains
        /**@brief Checks whether a certain key is contained in the sparse_map.
         * @param key Key to check for.
         * @returns bool true if the key was found, otherwise false.
         */
        L_NODISCARD bool contains(key_const_reference key) const
        {
            if (!m_sparse.count(key))
                return false;

            const size_type& sparseVal = m_sparse.at(key);
            return (sparseVal < m_size) && m_dense_key.at(sparseVal) == key;
        }

        /**@brief Checks if all keys in sparse_map are inside this map as well.
         * @param other Other sparse_map to check against.
         * @returns bool True if all keys in other are also in this sparse_map, otherwise false.
         */
        template<typename T>
        L_NODISCARD bool contains(const sparse_map<key_type, T>& other) const
        {
            if (other.m_size == 0)
                return true;

            if (m_size == 0 || m_size < other.m_size)
                return false;

            for (int i = 0; i < other.m_size; i++)
                if (!contains(other.m_dense_key.at(i)))
                    return false;

            return true;
        }
#pragma endregion

        /**@brief Checks if all keys and values and relations between them are the same for both sparse_maps.
         * @param other Other sparse_map to check against.
         * @returns bool True if both maps are the same size, contain the same keys, and all keys refer to the same value, otherwise false.
         */
        L_NODISCARD bool equals(self_const_reference other) const
        {
            if (m_size == other.m_size)
            {
                for (int i = 0; i < m_size; i++)
                    if (!(other.contains(m_dense_key.at(i)) && get(m_dense_key.at(i)) == other.get(m_dense_key.at(i))))
                        return false;

                return true;
            }

            return false;
        }

        /**@brief Checks if all keys and values and relations between them are the same for both sparse_maps.
         * @param other Other sparse_map to check against.
         * @returns bool True if both maps are the same size, contain the same keys, and all keys refer to the same value, otherwise false.
         */
        L_NODISCARD bool operator==(self_const_reference other) const
        {
            return equals(other);
        }

#pragma region find
        /**@brief Finds the iterator of a value using std::find.
         * @param val Value to find.
         * @returns Iterator to the value if found, otherwise end.
         */
        L_NODISCARD iterator find(value_const_reference val)
        {
            if (contains(val))
                return begin() + m_sparse.at(val);
            return end();
        }

        /**@brief Finds the iterator of a value using std::find.
         * @param val Value to find.
         * @returns Iterator to the value if found, otherwise end.
         */
        L_NODISCARD const_iterator find(value_const_reference val) const
        {
            if (contains(val))
                return begin() + m_sparse.at(val);
            return end();
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
            return emplace(key, val);
        }

        /**@brief Inserts new item into sparse_map
         * @param key Key to insert the new item to.
         * @param val Value to insert and link to the key.
         * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
         */
        std::pair<iterator, bool> insert(key_type&& key, value_const_reference val)
        {
            return emplace(std::move(key), val);
        }

        /**@brief Inserts new item into sparse_map
         * @param key Key to insert the new item to.
         * @param val Value to insert and link to the key.
         * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
         */
        std::pair<iterator, bool> insert(key_const_reference key, value_type&& val)
        {
            return emplace(key, std::move(val));
        }

        /**@brief Inserts new item into sparse_map
         * @param key Key to insert the new item to.
         * @param val Value to insert and link to the key.
         * @returns std::pair<iterator, bool> Iterator at the location of the key and true if succeeded, end and false if it didn't succeed.
         */
        std::pair<iterator, bool> insert(key_type&& key, value_type&& val)
        {
            return emplace(std::move(key), std::move(val));
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
                    reserve(m_capacity * static_cast<size_type>(2ull));

                m_dense_value.resize(m_size + static_cast<size_type>(1ull));
                m_dense_key.resize(m_size + static_cast<size_type>(1ull));

                auto valueItr = m_dense_value.begin() + m_size;
                auto keyItr = m_dense_key.begin() + m_size;

                new(&(*valueItr)) value_type(std::forward<Arguments>(arguments)...);
                new(&(*keyItr)) key_type(key);

                m_sparse.insert_or_assign(*keyItr, m_size);
                ++m_size;

                return std::make_pair(iterator(keyItr, valueItr), true);
            }

            return std::make_pair(end(), false);
        }

        /**@brief Construct item in place.
         * @param key Key to which the item should be created.
         * @param arguments Arguments to pass to the item constructor.
         */
        template<typename... Arguments>
        std::pair<iterator, bool> emplace(key_type&& key, Arguments&&... arguments)
        {
            byte tempBuffer[sizeof(key_type)];
            key_type* tempPtr = reinterpret_cast<key_type*>(&tempBuffer[0]);
            new(tempPtr) key_type(std::move(key));

            if (!contains(*tempPtr))
            {
                if (m_size >= m_capacity)
                    reserve(m_capacity * static_cast<size_type>(2ull));

                m_dense_value.resize(m_size + static_cast<size_type>(1ull));
                m_dense_key.resize(m_size + static_cast<size_type>(1ull));

                auto valueItr = m_dense_value.begin() + m_size;
                auto keyItr = m_dense_key.begin() + m_size;

                new(&(*valueItr)) value_type(std::forward<Arguments>(arguments)...);
                memcpy(&(*keyItr), tempPtr, sizeof(key_type));

                m_sparse.insert_or_assign(*keyItr, m_size);
                ++m_size;

                return std::make_pair(iterator(keyItr, valueItr), true);
            }

            return std::make_pair(end(), false);
        }
#pragma endregion

    private:
        inline L_ALWAYS_INLINE iterator _itr_at(index_type index) noexcept
        {
            return iterator(m_dense_key.begin() + index, m_dense_value.begin() + index);
        }

    public:

#pragma region try_emplace
        /**@brief Construct item in place.
         * @param key Key to which the item should be created.
         * @param arguments Arguments to pass to the item constructor.
         */
        template<typename... Arguments>
        std::pair<iterator, bool> try_emplace(key_const_reference key, Arguments&&... arguments)
        {
            if (!contains(key))
            {
                if (m_size >= m_capacity)
                    reserve(m_capacity * static_cast<size_type>(2ull));

                m_dense_value.resize(m_size + static_cast<size_type>(1ull));
                m_dense_key.resize(m_size + static_cast<size_type>(1ull));

                auto valueItr = m_dense_value.begin() + m_size;
                auto keyItr = m_dense_key.begin() + m_size;

                new(&(*valueItr)) value_type(std::forward<Arguments>(arguments)...);
                new(&(*keyItr)) key_type(key);

                m_sparse.insert_or_assign(*keyItr, m_size);
                ++m_size;

                return std::make_pair(iterator(keyItr, valueItr), true);
            }

            return std::make_pair(_itr_at(m_sparse.at(key)), false);
        }

        /**@brief Construct item in place.
         * @param key Key to which the item should be created.
         * @param arguments Arguments to pass to the item constructor.
         */
        template<typename... Arguments>
        std::pair<iterator, bool> try_emplace(key_type&& key, Arguments&&... arguments)
        {
            byte tempBuffer[sizeof(key_type)];
            key_type* tempPtr = reinterpret_cast<key_type*>(&tempBuffer[0]);
            new(tempPtr) key_type(std::move(key));

            if (!contains(*tempPtr))
            {
                if (m_size >= m_capacity)
                    reserve(m_capacity * static_cast<size_type>(2ull));

                m_dense_value.resize(m_size + static_cast<size_type>(1ull));
                m_dense_key.resize(m_size + static_cast<size_type>(1ull));

                auto valueItr = m_dense_value.begin() + m_size;
                auto keyItr = m_dense_key.begin() + m_size;

                new(&(*valueItr)) value_type(std::forward<Arguments>(arguments)...);
                memcpy(&(*keyItr), tempPtr, sizeof(key_type));

                m_sparse.insert_or_assign(*keyItr, m_size);
                ++m_size;

                return std::make_pair(iterator(keyItr, valueItr), true);
            }

            return std::make_pair(_itr_at(m_sparse.at(*tempPtr)), false);
        }

#pragma endregion


#pragma region operator[]
        /**@brief Returns item from sparse_map, inserts default value if it doesn't exist yet.
         * @param key Key value that needs to be retrieved.
         */
        value_reference operator[](key_type&& key)
        {
            byte tempBuffer[sizeof(key_type)];
            key_type* tempPtr = reinterpret_cast<key_type*>(&tempBuffer[0]);
            new(tempPtr) key_type(std::move(key));

            if (!contains(*tempPtr))
            {
                if (m_size >= m_capacity)
                    reserve(m_capacity * static_cast<size_type>(2ull));

                m_dense_value.resize(m_size + static_cast<size_type>(1ull));
                m_dense_key.resize(m_size + static_cast<size_type>(1ull));

                auto valueItr = m_dense_value.begin() + m_size;
                auto keyItr = m_dense_key.begin() + m_size;

                new(&(*valueItr)) value_type();
                memcpy(&(*keyItr), tempPtr, sizeof(key_type));

                m_sparse.insert_or_assign(*keyItr, m_size);
                ++m_size;
                return *valueItr;
            }

            return m_dense_value.at(m_sparse.at(*tempPtr));
        }

        /**@brief Returns item from sparse_map, inserts default value if it doesn't exist yet.
         * @param key Key value that needs to be retrieved.
         */
        value_reference operator[](key_const_reference key)
        {
            if (!contains(key))
            {
                if (m_size >= m_capacity)
                    reserve(m_capacity * static_cast<size_type>(2ull));

                m_dense_value.resize(m_size + static_cast<size_type>(1ull));
                m_dense_key.resize(m_size + static_cast<size_type>(1ull));

                auto valueItr = m_dense_value.begin() + m_size;
                auto keyItr = m_dense_key.begin() + m_size;

                new(&(*valueItr)) value_type();
                new(&(*keyItr)) key_type(key);

                m_sparse.insert_or_assign(*keyItr, m_size);
                ++m_size;
                return *valueItr;
            }

            return m_dense_value.at(m_sparse.at(key));
        }

        /**@brief Returns const item from const sparse_map.
         * @param key Key value that needs to be retrieved.
         */
        value_const_reference operator[](key_const_reference key) const
        {

#if defined(LEGION_DEBUG)
            if (!contains(key))
                throw std::out_of_range("Sparse map does not contain this key and is non modifiable.");
#endif

            return m_dense_value.at(m_sparse.at(key));
        }
#pragma endregion

#pragma region at
        /**@brief Returns item from sparse_map, throws exception if it doesn't exist yet.
         * @param key Key value that needs to be retrieved.
         */
        inline value_reference at(key_const_reference key)
        {
#if defined(LEGION_DEBUG)
            if (!contains(key))
                throw std::out_of_range("Sparse map does not contain this key");
#endif
            return m_dense_value.at(m_sparse.at(key));
        }

        /**@brief Returns const item from const sparse_map.
         * @param key Key value that needs to be retrieved.
         */
        inline value_const_reference at(key_const_reference key) const
        {
#if defined(LEGION_DEBUG)
            if (!contains(key))
                throw std::out_of_range("Sparse map does not contain this key");
#endif
            return m_dense_value.at(m_sparse.at(key));
        }
#pragma endregion

        /**@brief Erases item from sparse_map.
         * @param key Key value that needs to be erased.
         */
        size_type erase(key_const_reference key)
        {
            if (contains(key))
            {
                size_type& sparseVal = m_sparse.at(key);

                if (m_size - 1 != sparseVal)
                {
                    auto* denseValuePtr = &m_dense_value.at(m_size - 1);
                    auto* denseKeyPtr = &m_dense_key.at(m_size - 1);
                    size_type* sparsePtr = &m_sparse.at(*denseKeyPtr);
                    size_type lastSparse = *sparsePtr;

                    memcpy(&m_dense_value.at(sparseVal), denseValuePtr, sizeof(value_type));
                    memcpy(&m_dense_key.at(sparseVal), denseKeyPtr, sizeof(key_type));

                    *sparsePtr = sparseVal;
                    sparseVal = lastSparse;
                }

                --m_size;
                m_dense_value.resize(m_size);
                m_dense_key.resize(m_size);
                return true;
            }
            return false;
        }
    };
}
