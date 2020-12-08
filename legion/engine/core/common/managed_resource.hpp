#pragma once
#include <core/containers/delegate.hpp>
#include <core/async/readonly_rw_spinlock.hpp>
#include <unordered_map>

namespace legion::core::common
{
    template<typename T>
    struct managed_resource
    {
    private:
        static async::readonly_rw_spinlock m_referenceLock;
        static std::unordered_map<id_type, size_type> m_references;
        static std::atomic<id_type> m_lastId;
        id_type m_id = invalid_id;

        delegate<void(T&)> m_destroyFunc;

    public:
        T value;

        explicit managed_resource(nullptr_t t) {}

        template<typename... Args>
        managed_resource(delegate<void(T&)> destroyFunc, Args&&... args)
            : m_id(m_lastId.fetch_add(1, std::memory_order_acq_rel) + 1), m_destroyFunc(destroyFunc), value(std::forward(args)...)
        {
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
        }

        managed_resource(const managed_resource<T>& src)
            : m_id(src.m_id), m_destroyFunc(src.m_destroyFunc), m_copyFunc(src.m_copyFunc), m_moveFunc(src.m_moveFunc), value(m_copyFunc(src.value))
        {
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
        }

        managed_resource(managed_resource<T>&& src)
            : m_id(src.m_id), m_destroyFunc(src.m_destroyFunc), m_copyFunc(src.m_copyFunc), m_moveFunc(src.m_moveFunc), value(m_moveFunc(std::move(src.value)))
        {
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
        }

        managed_resource<T>& operator=(const managed_resource<T>& src)
        {
            m_id = src.m_id;
            m_destroyFunc = src.m_destroyFunc;
            m_copyFunc = src.m_copyFunc;
            m_moveFunc = src.m_moveFunc;
            value = m_copyFunc(src.value);
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
            return *this;
        }

        managed_resource<T>& operator=(managed_resource<T>&& src)
        {
            m_id = src.m_id;
            m_destroyFunc = src.m_destroyFunc;
            m_copyFunc = src.m_copyFunc;
            m_moveFunc = src.m_moveFunc;
            value = m_moveFunc(std::move(src.value));
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
            return *this;
        }

        T* operator&()
        {
            return &value;
        }

        operator T&()
        {
            return value;
        }

        operator const T&() const
        {
            return value;
        }

        ~managed_resource()
        {
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]--;
            if (m_references[m_id] == 0)
            {
                m_references.erase(m_id);
                m_destroyFunc(value);
            }
        }
    };
}
