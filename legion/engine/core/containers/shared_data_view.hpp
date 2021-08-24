#pragma once
#include <memory>
#include <utility>

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>

/**
 * @file shared_data_view.hpp
 */

namespace legion::core
{
    /**@class shared_data_view
     * @brief Reference counted owning view into an contiguous data grouping
     * @tparam DataType the value-type of the target data
     */
    template<typename DataType>
    struct shared_data_view
    {
    public:
        using value_type = DataType;
        using iterator = DataType*;

        using ptr_type = DataType*;
        using const_ptr_type = const DataType*;

        using const_value_type = const DataType;
        using const_iterator =  const DataType*;

        explicit shared_data_view(std::nullptr_t) noexcept;

        shared_data_view(ptr_type ptr, size_type size, diff_type offset = 0) noexcept;

        shared_data_view(const shared_data_view& other) noexcept;

        shared_data_view(shared_data_view&& other) noexcept;

        shared_data_view& operator=(const shared_data_view& other) noexcept;

        shared_data_view& operator=(shared_data_view&& other) noexcept;

        L_NODISCARD operator bool() const noexcept;

        L_NODISCARD bool operator==(const shared_data_view& other) const noexcept;
        L_NODISCARD bool operator!=(const shared_data_view& other) const noexcept;

        /**@brief gets reference to the value at index idx
         * checks if the index is valid before returning and throws and std::out_of_range exception if it is not
         * @param idx the index to query
         * @returns reference to the value at idx
         */
        L_NODISCARD value_type& at(size_type idx);

        /**@brief gets const reference to the value at index idx
         * checks if the index is valid before returning and throws and std::out_of_range exception if it is not
         * @param idx the index to query
         * @returns const reference to the value at idx
         */
        L_NODISCARD const value_type& at(size_type idx) const;

        /**@brief gets reference to the value at index idx
         * @param idx the index to query
         * @returns reference to the value at idx
         */
        value_type& operator[](size_type idx);

        /**@brief gets const reference to the value at index idx
         * @param idx the index to query
         * @returns const reference to the value at idx
         */
        L_NODISCARD const value_type& operator[](size_type idx) const;

        L_NODISCARD iterator begin() noexcept;

        L_NODISCARD iterator end() noexcept;

        L_NODISCARD ptr_type data() noexcept;

        L_NODISCARD const_iterator begin() const noexcept;

        L_NODISCARD const_iterator end() const noexcept;

        L_NODISCARD const_ptr_type data() const noexcept;

        /**@brief gets the size of the target data in item count.
         */
        L_NODISCARD size_type size() const noexcept;

        L_NODISCARD diff_type offset() const noexcept;

        /**@brief gets the max size this container could grow to.
         * since the container is non resizable same as size()
         */
        L_NODISCARD size_type max_size() const noexcept;

    private:
        std::shared_ptr<ptr_type> m_data;
        diff_type m_offset;
        size_type m_size;
    };
}

#include <core/containers/shared_data_view.inl>
