#pragma once

//Copyright (C) 2017 by Sergey A Kryukov: derived work
//http://www.SAKryukov.org
//http://www.codeproject.com/Members/SAKryukov

//Based on original work by Sergey Ryazanov:
//"The Impossibly Fast C++ Delegates", 18 Jul 2005
//https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

//MIT license:
//Copyright 2005 Sergey Ryazanov
//Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//The above copyright noticeand this permission notice shall be included in all copies or substantial portions of the Software.
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//Original publication: https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed


#include <vector>
#include <functional>
#include <utility>
#include <core/types/primitives.hpp>

namespace legion::core
{
    template<typename T>
    class delegate_base;

    template<typename return_type, typename ...parameter_types>
    class delegate_base<return_type(parameter_types...)>
    {
    protected:

        using stub_type = return_type(*)(void* this_ptr, parameter_types...);

        using allocator = void* (*)(void*);
        using deleter = void(*)(void*);

        struct invocation_element
        {
            invocation_element() = default;
            invocation_element(void* this_ptr, stub_type aStub, allocator aCopy = nullptr, allocator aMove = nullptr, deleter aDelete = nullptr) : object(this_ptr), stub(aStub), copy(aCopy), move(aMove), del(aDelete) {}
            invocation_element(const invocation_element& source)
            {
                if (source.copy != nullptr)
                {
                    object = source.copy(source.object);
                }
                else
                    object = source.object;
                stub = source.stub;
                copy = source.copy;
                move = source.move;
                del = source.del;
            }
            invocation_element(invocation_element&& source)
            {
                if (source.move != nullptr)
                {
                    object = source.move(source.object);
                }
                else
                    object = source.object;
                stub = source.stub;
                copy = source.copy;
                move = source.move;
                del = source.del;
            }

            ~invocation_element()
            {
                if (del != nullptr)
                    del(object);
            }

            invocation_element& operator=(const invocation_element& source)
            {
                if (source.copy != nullptr)
                {
                    object = source.copy(source.object);
                }
                else
                    object = source.object;
                stub = source.stub;
                copy = source.copy;
                move = source.move;
                del = source.del;
                return *this;
            }

            invocation_element& operator=(invocation_element&& source)
            {
                if (source.move != nullptr)
                {
                    object = source.move(source.object);
                }
                else
                    object = source.object;
                stub = source.stub;
                copy = source.copy;
                move = source.move;
                del = source.del;
                return *this;
            }

            void Clone(invocation_element& target) const
            {
                if (copy != nullptr)
                {
                    target.object = copy(object);
                }
                else
                    target.object = object;
                target.stub = stub;
                target.copy = copy;
                target.move = move;
                target.del = del;
            }

            bool operator ==(const invocation_element& other) const
            {
                return other.stub == stub && (object == other.object || (copy == other.copy && move == other.move && del == other.del));
            }
            bool operator !=(const invocation_element& other) const
            {
                return other.stub != stub || (object != other.object || (copy != other.copy || move != other.move || del != other.del));
            }

            void* object = nullptr;
            stub_type stub = nullptr;
            allocator copy = nullptr;
            allocator move = nullptr;
            deleter del = nullptr;
        };
    };


    template <typename T> class delegate;
    template <typename T> class multicast_delegate;

    template<typename return_type, typename ...parameter_types>
    class delegate<return_type(parameter_types...)> final : private delegate_base<return_type(parameter_types...)>
    {
    public:
        delegate() = default;
        delegate(delegate&&) = default;
        delegate& operator=(delegate&&) = default;
        delegate(const delegate& other) = default;

        bool isNull() const
        {
            return m_invocation.stub == nullptr;
        }

        bool operator ==(void* ptr) const
        {
            return (ptr == nullptr) && isNull();
        }
        bool operator !=(void* ptr) const
        {
            return (ptr != nullptr) || (!isNull());
        }

        delegate(std::nullptr_t)
        {
        }

        delegate& operator=(std::nullptr_t)
        {
            m_invocation.stub = nullptr;
            return *this;
        }

        void clear()
        {
            m_invocation.stub = nullptr;
        }

        template <typename lambda_type>
        delegate(const lambda_type& lambda)
        {
            assign((void*)(new lambda_type(lambda)), lambda_stub<lambda_type>,
                [](void* ptr)
                {
                    lambda_type* p = reinterpret_cast<lambda_type*>(ptr);
                    return (void*)(new lambda_type(*p));
                },
                [](void* ptr)
                {
                    lambda_type* p = reinterpret_cast<lambda_type*>(ptr);
                    return (void*)(new lambda_type(std::move(*p)));
                },
                [](void* ptr)
                {
                    lambda_type* p = reinterpret_cast<lambda_type*>(ptr);
                    delete p;
                });
        }

        delegate& operator =(const delegate& other)
        {
            other.m_invocation.Clone(m_invocation);
            return *this;
        }

        template <typename lambda_type>
        delegate& operator =(const lambda_type& instance)
        {
            assign((void*)(new lambda_type(instance)), lambda_stub<lambda_type>,
                [](void* ptr)
                {
                    lambda_type* p = reinterpret_cast<lambda_type*>(ptr);
                    return (void*)(new lambda_type(*p));
                },
                [](void* ptr)
                {
                    lambda_type* p = reinterpret_cast<lambda_type*>(ptr);
                    return (void*)(new lambda_type(std::move(*p)));
                },
                    [](void* ptr)
                {
                    lambda_type* p = reinterpret_cast<lambda_type*>(ptr);
                    delete p;
                });
            return *this;
        }

        bool operator == (const delegate& other) const
        {
            return m_invocation == other.m_invocation;
        }
        bool operator != (const delegate& other) const
        {
            return m_invocation != other.m_invocation;
        }

        bool operator ==(const multicast_delegate<return_type(parameter_types...)>& other) const
        {
            return other == (*this);
        }
        bool operator !=(const multicast_delegate<return_type(parameter_types...)>& other) const
        {
            return other != (*this);
        }

        template <class owner_type, return_type(owner_type::* func_type)(parameter_types...)>
        static delegate create(owner_type* instance)
        {
            return delegate(instance, method_stub<owner_type, func_type>);
        }

        template <class owner_type, return_type(owner_type::* func_type)(parameter_types...) const>
        static delegate create(owner_type const* instance)
        {
            return delegate(const_cast<owner_type*>(instance), const_method_stub<owner_type, func_type>);
        }

        template <return_type(*func_type)(parameter_types...)>
        static delegate create()
        {
            return delegate(nullptr, function_stub<func_type>);
        }

        template <typename lambda_type>
        static delegate create(const lambda_type& instance)
        {
            return delegate((void*)(new lambda_type(instance)), lambda_stub<lambda_type>);
        }

        return_type operator()(parameter_types... arguments) const
        {
            return (*m_invocation.stub)(m_invocation.object, arguments...);
        }
        return_type invoke(parameter_types... arguments) const
        {
            return (*m_invocation.stub)(m_invocation.object, arguments...);
        }

    private:
        delegate(void* anObject, typename delegate_base<return_type(parameter_types...)>::stub_type aStub)
        {
            m_invocation.object = anObject;
            m_invocation.stub = aStub;
        }

        void assign(void* anObject, typename delegate_base<return_type(parameter_types...)>::stub_type aStub,
            typename delegate_base<return_type(parameter_types...)>::allocator aCopy = nullptr,
            typename delegate_base<return_type(parameter_types...)>::allocator aMove = nullptr,
            typename delegate_base<return_type(parameter_types...)>::deleter aDelete = nullptr)
        {
            m_invocation.object = anObject;
            m_invocation.stub = aStub;
            m_invocation.copy = aCopy;
            m_invocation.move = aMove;
            m_invocation.del = aDelete;
        }

        template <class owner_type, return_type(owner_type::* func_type)(parameter_types...)>
        static return_type method_stub(void* this_ptr, parameter_types... arguments)
        {
            owner_type* p = static_cast<owner_type*>(this_ptr);
            return (p->*func_type)(arguments...);
        }

        template <class owner_type, return_type(owner_type::* func_type)(parameter_types...) const>
        static return_type const_method_stub(void* this_ptr, parameter_types... arguments)
        {
            owner_type* const p = static_cast<owner_type*>(this_ptr);
            return (p->*func_type)(arguments...);
        }

        template <return_type(*func_type)(parameter_types...)>
        static return_type function_stub(void* this_ptr, parameter_types... arguments)
        {
            return (func_type)(arguments...);
        }

        template <typename lambda_type>
        static return_type lambda_stub(void* this_ptr, parameter_types... arguments)
        {
            lambda_type* p = static_cast<lambda_type*>(this_ptr);
            return (p->operator())(arguments...);
        }

        friend class multicast_delegate<return_type(parameter_types...)>;
        typename delegate_base<return_type(parameter_types...)>::invocation_element m_invocation;
    };

    template<typename return_type, typename ...parameter_types>
    class multicast_delegate<return_type(parameter_types...)> final : private delegate_base<return_type(parameter_types...)>
    {
    public:

        multicast_delegate() = default;
        multicast_delegate(multicast_delegate&&) = default;
        multicast_delegate& operator=(multicast_delegate&&) = default;
        multicast_delegate(const multicast_delegate& other) = default;

        multicast_delegate& operator =(const multicast_delegate& other)
        {
            m_invocationList.clear();

            for (auto& invocation : other.m_invocationList)
                m_invocationList.emplace_back(invocation);

            return *this;
        }

        ~multicast_delegate()
        {
            m_invocationList.clear();
        }

        bool isNull() const
        {
            return m_invocationList.size() < 1;
        }

        size_type size() const
        {
            return m_invocationList.size();
        }

        bool operator ==(void* ptr) const
        {
            return (ptr == nullptr) && isNull();
        }
        bool operator !=(void* ptr) const
        {
            return (ptr != nullptr) || (!isNull());
        }

        bool operator ==(const multicast_delegate& other) const
        {
            if (m_invocationList.size() != other.m_invocationList.size())
                return false;

            auto anotherIt = other.m_invocationList.begin();
            for (auto it = m_invocationList.begin(); it != m_invocationList.end(); ++it)
                if (*it != *anotherIt) return false;

            return true;
        }

        bool operator !=(const multicast_delegate& other) const
        {
            return !(*this == other);
        }

        bool operator ==(const delegate<return_type(parameter_types...)>& other) const
        {
            if (isNull() && other.isNull())
                return true;
            if (other.isNull() || (size() != 1))
                return false;

            return (other.m_invocation == *m_invocationList.begin());
        }

        bool operator !=(const delegate<return_type(parameter_types...)>& other) const
        {
            return !(*this == other);
        }

        multicast_delegate& operator +=(const multicast_delegate& other)
        {
            for (auto& item : other.m_invocationList) // clone, not copy; flattens hierarchy:
                m_invocationList.emplace_back(item.object, item.stub);
            return *this;
        }

        template <typename lambda_type>
        multicast_delegate& operator +=(const lambda_type& lambda)
        {
            delegate<return_type(parameter_types...)> d = delegate<return_type(parameter_types...)>::template create<lambda_type>(lambda);
            return *this += d;
        }

        multicast_delegate& operator +=(const delegate<return_type(parameter_types...)>& other)
        {
            if (other.isNull())
                return *this;

            m_invocationList.emplace_back(other.m_invocation.object, other.m_invocation.stub);
            return *this;
        }

        void operator()(parameter_types... arguments) const
        {
            for (auto& item : m_invocationList)
                (*(item.stub))(item.object, arguments...);
        }

        void invoke(parameter_types... arguments) const
        {
            for (auto& item : m_invocationList)
                (*(item.stub))(item.object, arguments...);
        }

        template<typename return_handler>
        void operator()(parameter_types... arguments, return_handler handler) const
        {
            size_type index = 0;
            for (auto& item : m_invocationList)
            {
                return_type value = (*(item.stub))(item.object, arguments...);
                handler(index, &value);
                ++index;
            }
        }

        template<typename return_handler>
        void invoke(parameter_types... arguments, return_handler handler) const
        {
            size_type index = 0;
            for (auto& item : m_invocationList)
            {
                return_type value = (*(item.stub))(item.object, arguments...);
                handler(index, &value);
                ++index;
            }
        }

        void operator()(parameter_types... arguments, delegate<void(size_type, return_type*)> handler) const
        {
            operator() < decltype(handler) > (arguments..., handler);
        }

        void invoke(parameter_types... arguments, delegate<void(size_type, return_type*)> handler) const
        {
            operator() < decltype(handler) > (arguments..., handler);
        }

        void operator()(parameter_types... arguments, std::function<void(size_type, return_type*)> handler) const
        {
            operator() < decltype(handler) > (arguments..., handler);
        }

        void invoke(parameter_types... arguments, std::function<void(size_type, return_type*)> handler) const
        {
            operator() < decltype(handler) > (arguments..., handler);
        }

    private:
        std::vector<typename delegate_base<return_type(parameter_types...)>::invocation_element> m_invocationList;
    };
}
