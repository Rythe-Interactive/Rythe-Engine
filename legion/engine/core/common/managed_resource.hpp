#pragma once
#include <core/containers/delegate.hpp>
#include <core/async/rw_spinlock.hpp>
#include <unordered_map>

#include <Optick/optick.h>

namespace legion::core::common
{
    template<typename T>
    struct managed_resource
    {
    private:
        static async::rw_spinlock m_referenceLock;
        static std::unordered_map<id_type, size_type> m_references;
        static std::atomic<id_type> m_lastId;
        id_type m_id = invalid_id;

        delegate<void(T&)> m_destroyFunc;

    public:
        T value;

        explicit managed_resource(std::nullptr_t t) : value() {}

        template<typename... Args>
        managed_resource(delegate<void(T&)> destroyFunc, Args&&... args)
            : m_id(m_lastId.fetch_add(1, std::memory_order_acq_rel) + 1), m_destroyFunc(destroyFunc), value(std::forward<Args>(args)...)
        {
            OPTICK_EVENT();
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
        }

        managed_resource(const managed_resource<T>& src)
            : m_id(src.m_id), m_destroyFunc(src.m_destroyFunc), value(src.value)
        {
            OPTICK_EVENT();
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
        }

        managed_resource(managed_resource<T>&& src)
            : m_id(src.m_id), m_destroyFunc(std::move(src.m_destroyFunc)), value(std::move(src.value))
        {
            OPTICK_EVENT();
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
        }

        managed_resource<T>& operator=(const managed_resource<T>& src)
        {
            OPTICK_EVENT();
            m_id = src.m_id;
            m_destroyFunc = src.m_destroyFunc;
            value = src.value;
            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]++;
            return *this;
        }

        managed_resource<T>& operator=(managed_resource<T>&& src)
        {
            OPTICK_EVENT();
            m_id = src.m_id;
            m_destroyFunc = std::move(src.m_destroyFunc);
            value = std::move(src.value);
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
            OPTICK_EVENT();
            if (!m_id)
                return;

            async::readwrite_guard guard(m_referenceLock);
            m_references[m_id]--;
            if (m_references[m_id] == 0)
            {
                m_references.erase(m_id);
                m_destroyFunc(value);
            }
        }
    };

    template<typename T>
    async::rw_spinlock managed_resource<T>::m_referenceLock;

    template<typename T>
    std::unordered_map<id_type, size_type> managed_resource<T>::m_references;

    template<typename T>
    std::atomic<id_type> managed_resource<T>::m_lastId;
}
