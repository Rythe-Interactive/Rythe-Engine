#pragma once

#include <vector>
#include <core/platform/platform.hpp>

/**
 * @file vector_view.hpp
 */

namespace legion::core
{

    template <class T,bool is_const = false>
    class vector_view
    {
    public:
        using vector_type = std::vector<T>;
        using size_type = typename vector_type::size_type;
        using difference_type = typename vector_type::difference_type;

        using iterator = typename vector_type::iterator;
        using const_iterator = typename vector_type::const_iterator;
        using size_type = typename vector_type::size_type;

        explicit vector_view(std::nullptr_t) : m_targetVector(nullptr) {}

        explicit vector_view(typename std::conditional<is_const,const vector_type*,vector_type*>::type tv,size_type size,difference_type offset) :
            m_targetVector(tv),
            m_targetSize(size),
            m_offset(offset)
        {}

        vector_view(const vector_view&) = default;
        vector_view(vector_view&&) noexcept = default;

        vector_view& operator=(vector_view&& other) noexcept
        {
            m_targetVector = std::move(other.m_targetVector);
            m_targetSize = std::move(other.m_targetSize);
            m_offset = std::move(other.m_offset);            
            return *this;
        }

        vector_view& operator=(const vector_view& other)
        {
            if (this == &other)
                return *this;
            m_targetVector = other.m_targetVector;
            m_targetSize = other.m_targetSize;
            m_offset = other.m_offset;
            return *this;
        }

        ~vector_view() = default;

        template< typename = std::enable_if_t<is_const>>
        L_NODISCARD iterator begin()
        {
            return m_targetVector->begin() + m_offset;
        }

        L_NODISCARD const_iterator begin() const 
        {
            return m_targetVector->cbegin() + m_offset;
        }

        template< typename = std::enable_if_t<is_const>>
        L_NODISCARD iterator end()
        {
            return m_targetVector->begin() + m_offset + m_targetSize;
        }

        L_NODISCARD const_iterator end() const 
        {
            return m_targetVector->cbegin() + m_offset + m_targetSize;
        }

        template< typename = std::enable_if_t<is_const>>
        L_NODISCARD auto data()
        {
            return m_targetVector->data() + m_offset;
        }

        L_NODISCARD auto data() const -> const T*
        {
            return m_targetVector->data() + m_offset;
        }

        L_NODISCARD auto size() const noexcept
        {
            return m_targetSize;
        }

        L_NODISCARD auto max_size() const noexcept
        {
            return m_targetSize;
        }

    private:
        std::conditional<is_const,const vector_type,vector_type>* m_targetVector;
        size_type m_targetSize;
        difference_type m_offset;
    };
}
