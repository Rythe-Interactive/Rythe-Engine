#pragma once
#include <cstdint>
#include <cstddef>
#include <atomic>
#include <stdexcept>

#include "core/platform/platform.hpp"

/**
 * @file data_view.hpp
 */

namespace legion::core
{
    /**@class data_view
     * @brief implementation if lpe array-view is not available
     * a reference-counting view into a raw array, implements most of the required things
     * for a standard stl container
     * @tparam DataType the value-type of the target_array
     */
    template <class DataType>
    class data_view
    {
    public:

        using value_type = DataType;
        using iterator = DataType*;

        using ptr_type = DataType*;
        using const_ptr_type = const DataType *;

        using size_type = std::size_t;


        using const_value_type = const DataType;
        using const_iterator =  const DataType *;

        explicit data_view(nullptr_t) : data_view(nullptr,0,0,false){}

        explicit data_view(ptr_type ptr,size_type size,size_type offset = 0,bool take_ownership = false) :
            m_targetArray(ptr),
            m_targetOffset(offset),
            m_targetSize(size)
        {
            if(take_ownership)
                m_referenceCount = new std::atomic<uint32_t>(1);
        }

        data_view(const data_view& other) :
            m_referenceCount(other.m_referenceCount),
            m_targetArray(other.m_targetArray),
            m_targetOffset(other.m_targetOffset),
            m_targetSize(other.m_targetSize)
        {
            try_inc_ref_count();
        }

        data_view(data_view&& other) noexcept:
            m_referenceCount(std::move(other.m_referenceCount)),
            m_targetArray(std::move(other.m_targetArray)),
            m_targetOffset(std::move(other.m_targetOffset)),
            m_targetSize(std::move(other.m_targetSize))
        {
          try_inc_ref_count();
        }


        data_view& operator=(const data_view& other)
        {
            if (this == &other)
                return *this;
            m_referenceCount = other.m_referenceCount;
            m_targetArray = other.m_targetArray;
            m_targetOffset = other.m_targetOffset;
            m_targetSize = other.m_targetSize;

            try_inc_ref_count();

            return *this;
        }

        data_view& operator=(data_view&& other) noexcept
        {
            if (this == &other)
                return *this;
            m_referenceCount = other.m_referenceCount;
            m_targetArray = other.m_targetArray;
            m_targetOffset = other.m_targetOffset;
            m_targetSize = other.m_targetSize;

            try_inc_ref_count();

            return *this;
        }


        ~data_view()
        {
            if(m_referenceCount)
            {
                if(try_dec_ref_count() == 0)
                {
                    //TODO(algo-ryth-mix): there might be a bug here but it is really hard to tell for me:
                    //TODO(cont.):         basically we are deleting the atomic without being in a critical section
                    //TODO(cont.):         which means we could be deleting something, that someone else wants to access
                    //TODO(cont.):         but we are also deleting with the assumption that we are the last element (ref_count was 1)
                    //TODO(cont.):         so I am not quite sure what is going on here
                    delete[] m_targetArray;
                    delete m_referenceCount;   
                }
            }
        }

        /** @brief gets the value at index idx
	     *  checks if the index is valid before returning and throws and std::out_of_range exception if it is not
	     *  @param idx the index to query
	     *  @returns the value at idx
	     */
        L_NODISCARD value_type at(size_type idx) const
        {
            if(idx > m_targetSize) throw std::out_of_range("data_view subscript out of range");
            return this->operator[](idx);
        }

        /** @brief gets the value at index idx
	     *  @param idx the index to query
	     *  @returns the value at idx
	     */	
        value_type& operator[](size_type idx)
        {
            if(idx > m_targetSize) throw std::out_of_range("data_view subscript out of range");
            return *(m_targetArray + m_targetOffset + static_cast<ptrdiff_t>(idx));
        }

        /**@brief const version of above
	     * @see value_type& operator[](size_type)
	     */
        L_NODISCARD value_type operator[](size_type idx) const
        {
            if(idx > m_targetSize) throw std::out_of_range("data_view subscript out of range");
            return *(m_targetArray + m_targetOffset + static_cast<ptrdiff_t>(idx));
        }

        L_NODISCARD iterator begin()
        {
            return m_targetArray + m_targetOffset;
        }

        L_NODISCARD iterator end()
        {
            return begin() + m_targetSize;
        }

        L_NODISCARD ptr_type data()
        {
            return begin();
        }

        L_NODISCARD const_iterator begin() const
        {
            return m_targetArray + m_targetOffset;
        }

        L_NODISCARD const_iterator end() const
        {
            return begin() + m_targetSize;
        }

        L_NODISCARD const_ptr_type data() const
        {
            return begin();
        }

        /**@brief gets the size of the target array-view
	     */
        L_NODISCARD size_type size() const noexcept
        {
            return m_targetSize;
        }

        /**@brief gets the max size this container could grow to.
	     * since the container is non resizable same as size()
	     */
        L_NODISCARD size_type max_size() const noexcept
        {
            return m_targetSize;
        }

    private:

        void try_inc_ref_count() const
        {
            if(m_referenceCount)
            {
                m_referenceCount->fetch_add(1);
            }
        }

        uint32_t try_dec_ref_count() const
        {
            if(m_referenceCount)
            {
                // for some reason c++ calls execution order fetch_sub does exactly that, it fetches first, subtracts then and returns what it fetched beforehand
                return m_referenceCount->fetch_sub(1) - 1;
            }
            return 1;
        }


        mutable std::atomic<uint32_t>* m_referenceCount = nullptr;
        ptr_type m_targetArray;
        ptrdiff_t m_targetOffset;
        size_type m_targetSize;
    };
}
