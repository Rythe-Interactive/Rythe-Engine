#pragma once
#include <cassert>
#include <algorithm>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/containers/iterator_tricks.hpp>

/**
 * @file delegate.hpp
 */

namespace legion::core
{
    /**@struct delegate
    * @brief
    * @tparam Func
    */
    template <typename Func> struct delegate;

    /**@struct multicast_delegate
     * @brief
     * @tparam Func
     */
    template <typename Func> struct multicast_delegate;

    template<typename R, typename... Args>
    struct delegate<R(Args...)>
    {
    public:
        using return_type = R;
        using argument_types = std::tuple<Args...>;
        using stub_ptr_type = return_type(*)(void*, Args&&...);
        using invocable_type = return_type(Args...);

#pragma region constructors
    private:
        delegate(void* const o, stub_ptr_type const m) noexcept :
            object_ptr_(o),
            stub_ptr_(m)
        {
        }

    public:
        delegate() noexcept = default;

        delegate(delegate const&) noexcept = default;

        delegate(delegate&&) noexcept = default;

        delegate(std::nullptr_t const) noexcept : delegate() { }

        template <typename owner_type CNDOXY(typename = typename std::enable_if_t<std::is_class_v<owner_type>>)>
        explicit delegate(owner_type const* const o) noexcept : object_ptr_(const_cast<owner_type*>(o)) {}

        template <typename owner_type CNDOXY(typename = typename std::enable_if_t<std::is_class_v<owner_type>>)>
        explicit delegate(owner_type const& o) noexcept : object_ptr_(const_cast<owner_type*>(&o)) {}

        template <typename owner_type>
        delegate(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...)) 
        {
            *this = from(object_ptr, method_ptr);
        }

        template <typename owner_type>
        delegate(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            *this = from(object_ptr, method_ptr);
        }

        template <typename owner_type>
        delegate(owner_type& object, return_type(owner_type::* const method_ptr)(Args...))
        {
            *this = from(object, method_ptr);
        }

        template <typename owner_type>
        delegate(owner_type const& object, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            *this = from(object, method_ptr);
        }

        template <typename T CNDOXY(typename = typename std::enable_if_t<!std::is_same_v<delegate, typename std::decay_t<T>>>)>
        delegate(T&& f) :
            store_(operator new(sizeof(typename std::decay_t<T>)), functor_deleter<typename std::decay_t<T>>),
            store_size_(sizeof(typename std::decay_t<T>))
        {
            using functor_type = typename std::decay_t<T>;

            new (store_.get()) functor_type(std::forward<T>(f));

            object_ptr_ = store_.get();

            stub_ptr_ = functor_stub<functor_type>;

            deleter_ = deleter_stub<functor_type>;
        }
#pragma endregion

#pragma region assignment
        delegate& operator=(delegate const&) = default;

        delegate& operator=(delegate&&) = default;

        delegate& operator=(std::nullptr_t)
        {
            reset();
            return *this;
        }

        template <typename owner_type>
        delegate& operator=(return_type(owner_type::* const rhs)(Args...))
        {
            return *this = from(static_cast<owner_type*>(object_ptr_), rhs);
        }

        template <typename owner_type>
        delegate& operator=(return_type(owner_type::* const rhs)(Args...) const)
        {
            return *this = from(static_cast<owner_type const*>(object_ptr_), rhs);
        }

        template<typename T CNDOXY(typename = typename std::enable_if_t<!std::is_same_v<delegate, typename std::decay_t<T>>>)>
        delegate& operator=(T&& f)
        {
            using functor_type = typename std::decay<T>::type;

            if ((sizeof(functor_type) > store_size_) || !store_.unique())
            {
                store_.reset(operator new(sizeof(functor_type)),
                    functor_deleter<functor_type>);

                store_size_ = sizeof(functor_type);
            }
            else
            {
                deleter_(store_.get());
            }

            new (store_.get()) functor_type(std::forward<T>(f));

            object_ptr_ = store_.get();

            stub_ptr_ = functor_stub<functor_type>;

            deleter_ = deleter_stub<functor_type>;

            return *this;
        }
#pragma endregion

#pragma region from
        template <return_type(* const function_ptr)(Args...)>
        static delegate from() noexcept
        {
            return { nullptr, function_stub<function_ptr> };
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...)>
        static delegate from(owner_type* const object_ptr) noexcept
        {
            return { object_ptr, method_stub<owner_type, method_ptr> };
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...) const>
        static delegate from(owner_type const* const object_ptr) noexcept
        {
            return { const_cast<owner_type*>(object_ptr), const_method_stub<owner_type, method_ptr> };
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...)>
        static delegate from(owner_type& object) noexcept
        {
            return { &object, method_stub<owner_type, method_ptr> };
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...) const>
        static delegate from(owner_type const& object) noexcept
        {
            return { const_cast<owner_type*>(&object), const_method_stub<owner_type, method_ptr> };
        }

        template <typename T>
        static delegate from(T&& f)
        {
            return std::forward<T>(f);
        }

        static delegate from(return_type(* const function_ptr)(Args...))
        {
            return function_ptr;
        }

        template <typename owner_type>
        using member_pair = std::pair<owner_type* const, return_type(owner_type::* const)(Args...)>;

        template <typename owner_type>
        using const_member_pair = std::pair<owner_type const* const, return_type(owner_type::* const)(Args...) const>;

        template <typename owner_type>
        static delegate from(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...))
        {
            return member_pair<owner_type>(object_ptr, method_ptr);
        }

        template <typename owner_type>
        static delegate from(owner_type const* const object_ptr, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            return const_member_pair<owner_type>(object_ptr, method_ptr);
        }

        template <typename owner_type>
        static delegate from(owner_type& object, return_type(owner_type::* const method_ptr)(Args...))
        {
            return member_pair<owner_type>(&object, method_ptr);
        }

        template <typename owner_type>
        static delegate from(owner_type const& object, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            return const_member_pair<owner_type>(&object, method_ptr);
        }
#pragma endregion

        void reset() { stub_ptr_ = nullptr; store_.reset(); }

        void reset_stub() noexcept { stub_ptr_ = nullptr; }

        void swap(delegate& other) noexcept { std::swap(*this, other); }

#pragma region comparison
        bool operator==(delegate const& rhs) const noexcept
        {
            return (object_ptr_ == rhs.object_ptr_) && (stub_ptr_ == rhs.stub_ptr_);
        }

        bool operator!=(delegate const& rhs) const noexcept
        {
            return !operator==(rhs);
        }

        bool operator<(delegate const& rhs) const noexcept
        {
            return (object_ptr_ < rhs.object_ptr_) ||
                ((object_ptr_ == rhs.object_ptr_) && (stub_ptr_ < rhs.stub_ptr_));
        }

        bool operator==(std::nullptr_t const) const noexcept
        {
            return !stub_ptr_;
        }

        bool operator!=(std::nullptr_t const) const noexcept
        {
            return stub_ptr_;
        }
#pragma endregion

        explicit operator bool() const noexcept { return stub_ptr_; }

        auto invoke(Args... args) const noexcept(
            (std::is_same_v<return_type, void> ||
                std::is_default_constructible_v<return_type> ||
                std::is_constructible_v<return_type, std::nullptr_t>) &&
            noexcept(stub_ptr_(std::declval<decltype(object_ptr_)>(), std::declval<Args>()...)))
        {
            if constexpr (std::is_same_v<return_type, void>)
            {
                if (stub_ptr_)
                    stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
            else if constexpr (std::is_default_constructible_v<return_type>)
            {
                if (!stub_ptr_)
                    return return_type{};
                return stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
            else if constexpr (std::is_constructible_v<return_type, std::nullptr_t>)
            {
                if (!stub_ptr_)
                    return return_type{ nullptr };
                return stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
            else
            {
                return stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
        }

        auto operator()(Args... args) const noexcept(
            (std::is_same_v<return_type, void> ||
                std::is_default_constructible_v<return_type> ||
                std::is_constructible_v<return_type, std::nullptr_t>) &&
            noexcept(stub_ptr_(std::declval<decltype(object_ptr_)>(), std::declval<Args>()...)))
        {
            if constexpr (std::is_same_v<return_type, void>)
            {
                if (stub_ptr_)
                    stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
            else if constexpr (std::is_default_constructible_v<return_type>)
            {
                if (!stub_ptr_)
                    return return_type{};
                return stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
            else
            {
                return stub_ptr_(object_ptr_, std::forward<Args>(args)...);
            }
        }

#pragma region privates
    private:
        friend struct std::hash<delegate>;

        using deleter_type = void (*)(void*);

        void* object_ptr_;
        stub_ptr_type stub_ptr_{};

        deleter_type deleter_;

        std::shared_ptr<void> store_;
        std::size_t store_size_;

        template <typename T>
        static void functor_deleter(void* const p)
        {
            static_cast<T*>(p)->~T();

            operator delete(p);
        }

#pragma region stubs
        template <typename T>
        static void deleter_stub(void* const p)
        {
            static_cast<T*>(p)->~T();
        }

        template <return_type(*function_ptr)(Args...)>
        static return_type function_stub(void* const, Args&&... args)
        {
            return function_ptr(std::forward<Args>(args)...);
        }

        template <typename owner_type, return_type(owner_type::* method_ptr)(Args...)>
        static return_type method_stub(void* const object_ptr, Args&&... args)
        {
            return (static_cast<owner_type*>(object_ptr)->*method_ptr)(
                std::forward<Args>(args)...);
        }

        template <typename owner_type, return_type(owner_type::* method_ptr)(Args...) const>
        static return_type const_method_stub(void* const object_ptr, Args&&... args)
        {
            return (static_cast<owner_type const*>(object_ptr)->*method_ptr)(
                std::forward<Args>(args)...);
        }

        template <typename>
        struct is_member_pair : std::false_type { };

        template <typename owner_type>
        struct is_member_pair<std::pair<owner_type* const, return_type(owner_type::* const)(Args...)>> : std::true_type
        {
        };

        template <typename owner_type>
        static constexpr bool is_member_pair_v = is_member_pair<owner_type>::value;

        template <typename>
        struct is_const_member_pair : std::false_type { };

        template <typename owner_type>
        struct is_const_member_pair<std::pair<owner_type const* const, return_type(owner_type::* const)(Args...) const>> : std::true_type
        {
        };

        template <typename owner_type>
        static constexpr bool is_const_member_pair_v = is_const_member_pair<owner_type>::value;

        template <typename T>
        static std::enable_if_t<!(is_member_pair_v<T> || is_const_member_pair_v<T>), return_type>
            functor_stub(void* const object_ptr, Args&&... args)
        {
            return (*static_cast<T*>(object_ptr))(std::forward<Args>(args)...);
        }

        template <typename T>
        static std::enable_if_t<is_member_pair_v<T> || is_const_member_pair_v<T>, return_type>
            functor_stub(void* const object_ptr, Args&&... args)
        {
            return (static_cast<T*>(object_ptr)->first->*
                static_cast<T*>(object_ptr)->second)(std::forward<Args>(args)...);
        }
#pragma endregion
#pragma endregion
    };

    template<typename R, typename ...Args>
    struct multicast_delegate<R(Args...)>
    {
    public:
        using value_type = delegate<R(Args...)>;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;

        using invocation_type = value_type;
        using return_type = typename invocation_type::return_type;
        using argument_types = typename invocation_type::argument_types;
        using stub_ptr_type = typename invocation_type::stub_ptr_type;
        using invocable_type = typename invocation_type::invocable_type;

        template<typename T>
        using container_type = std::vector<T>;
        using underlying_type = container_type<value_type>;

        using iterator = typename underlying_type::iterator;
        using const_iterator = typename underlying_type::const_iterator;
        using reverse_iterator = typename underlying_type::reverse_iterator;
        using const_reverse_iterator = typename underlying_type::const_reverse_iterator;
        using reverse_itr_range = pair_range<reverse_iterator>;
        using const_reverse_itr_range = pair_range<const_reverse_iterator>;

#pragma region element access
        L_NODISCARD reference at(size_type index) { return m_invocationList.at(index); }
        L_NODISCARD const_reference at(size_type index) const { return m_invocationList.at(index); }

        L_NODISCARD reference operator[](size_type index) { return m_invocationList[index]; }
        L_NODISCARD const_reference operator[](size_type index) const { return m_invocationList[index]; }

        L_NODISCARD reference front() { return m_invocationList.front(); }
        L_NODISCARD const_reference front() const { return m_invocationList.front(); }

        L_NODISCARD reference back() { return m_invocationList.back(); }
        L_NODISCARD const_reference back() const { return m_invocationList.back(); }

        L_NODISCARD pointer data() { return m_invocationList.data(); }
        L_NODISCARD const_pointer data() const { return m_invocationList.data(); }
#pragma endregion

#pragma region iterators
        L_NODISCARD iterator begin() noexcept { return m_invocationList.begin(); }
        L_NODISCARD const_iterator begin() const noexcept { return m_invocationList.cbegin(); }
        L_NODISCARD const_iterator cbegin() const noexcept { return m_invocationList.cbegin(); }

        L_NODISCARD iterator end() noexcept { return m_invocationList.end(); }
        L_NODISCARD const_iterator end() const noexcept { return m_invocationList.cend(); }
        L_NODISCARD const_iterator cend() const noexcept { return m_invocationList.cend(); }

        L_NODISCARD reverse_iterator rbegin() noexcept { return m_invocationList.rbegin(); }
        L_NODISCARD const_reverse_iterator rbegin() const noexcept { return m_invocationList.crbegin(); }
        L_NODISCARD const_reverse_iterator crbegin() const noexcept { return m_invocationList.crbegin(); }

        L_NODISCARD reverse_iterator rend() noexcept { return m_invocationList.rend(); }
        L_NODISCARD const_reverse_iterator rend() const noexcept { return m_invocationList.crend(); }
        L_NODISCARD const_reverse_iterator crend() const noexcept { return m_invocationList.crend(); }

        L_NODISCARD reverse_itr_range reverse_range() noexcept { return pair_range{ rbegin(), rend() }; }
        L_NODISCARD const_reverse_itr_range reverse_range() const noexcept { return pair_range{ crbegin(), crend() }; }
#pragma endregion

#pragma region capacity
        L_NODISCARD bool empty() const noexcept { return m_invocationList.empty(); }
        L_NODISCARD size_type size() const noexcept { return m_invocationList.size(); }
        L_NODISCARD size_type max_size() const noexcept { return m_invocationList.max_size(); }
        void reserve(size_type newSize) { m_invocationList.reserve(newSize); }
        L_NODISCARD size_type capacity() const noexcept { return m_invocationList.capacity(); }
        void shrink_to_fit() { m_invocationList.shrink_to_fit(); }
#pragma endregion

#pragma region modifiers
#pragma region assign
        void assign(size_type count, const_reference value) { m_invocationList.assign(count, value); }
        template<typename InputIt>
        void assign(InputIt first, InputIt last) { m_invocationList.assign(first, last); }
        void assign(std::initializer_list<value_type> ilist) { m_invocationList.assign(ilist); }
#pragma endregion

        void clear() noexcept { m_invocationList.clear(); }

#pragma region insert
        iterator insert(const_iterator pos, const_reference value) { return m_invocationList.insert(pos, value); }
        iterator insert(const_iterator pos, value_type&& value) { return m_invocationList.insert(pos, std::move(value)); }
        iterator insert(const_iterator pos, size_type count, const_reference value) { return m_invocationList.insert(pos, count, value); }
        template<typename InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) { return m_invocationList.insert(pos, first, last); }
        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) { return m_invocationList.insert(pos, ilist); }
#pragma endregion

        template<typename... CtrArgs>
        iterator emplace(const_iterator pos, CtrArgs&&... args) { return m_invocationList.emplace(pos, std::forward<CtrArgs>(args)...); }

#pragma region erase
        size_type erase(const invocation_type& value)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                value
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <return_type(* const function_ptr)(Args...)>
        size_type erase()
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<function_ptr>()
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...)>
        size_type erase(owner_type* const object_ptr)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type, method_ptr>(object_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...) const>
        size_type erase(owner_type const* const object_ptr)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type, method_ptr>(object_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...)>
        size_type erase(owner_type& object)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type, method_ptr>(object)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...) const>
        size_type erase(owner_type const& object)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type, method_ptr>(object)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename T>
        size_type erase(T&& f)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::from(std::forward<T>(f))
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        size_type erase(return_type(* const function_ptr)(Args...))
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::from(function_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type>
        size_type erase(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...))
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type>(object_ptr, method_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type>
        size_type erase(owner_type const* const object_ptr, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type>(object_ptr, method_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type>
        size_type erase(owner_type& object, return_type(owner_type::* const method_ptr)(Args...))
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type>(object, method_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }

        template <typename owner_type>
        size_type erase(owner_type const& object, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            auto size = m_invocationList.size();
            m_invocationList.erase(std::remove(m_invocationList.begin(), m_invocationList.end(),
                invocation_type::template from<owner_type>(object, method_ptr)
            ), m_invocationList.end());
            return size - m_invocationList.size();
        }
#pragma endregion

#pragma region push_back
        void push_back(const_reference value) { m_invocationList.push_back(value); }
        void push_back(value_type&& value) { m_invocationList.push_back(value); }
#pragma endregion

#pragma region insert_back
        void insert_back(const invocation_type& value)
        {
            m_invocationList.push_back(value);
        }

        template <return_type(* const function_ptr)(Args...)>
        void insert_back()
        {
            m_invocationList.push_back(invocation_type::template from<function_ptr>());
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...)>
        void insert_back(owner_type* const object_ptr)
        {
            m_invocationList.push_back(invocation_type::template from<owner_type, method_ptr>(object_ptr));
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...) const>
        void insert_back(owner_type const* const object_ptr)
        {
            m_invocationList.push_back(invocation_type::template from<owner_type, method_ptr>(object_ptr));
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...)>
        void insert_back(owner_type& object)
        {
            m_invocationList.push_back(invocation_type::template from<owner_type, method_ptr>(object));
        }

        template <typename owner_type, return_type(owner_type::* const method_ptr)(Args...) const>
        void insert_back(owner_type const& object)
        {
            m_invocationList.push_back(invocation_type::template from<owner_type, method_ptr>(object));
        }

        template <typename T>
        void insert_back(T&& f)
        {
            m_invocationList.push_back(invocation_type::from(std::forward<T>(f)));
        }

        void insert_back(return_type(* const function_ptr)(Args...))
        {
            m_invocationList.push_back(invocation_type::from(function_ptr));
        }

        template <typename owner_type>
        void insert_back(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...))
        {
            m_invocationList.push_back(invocation_type::template from<owner_type>(object_ptr, method_ptr));
        }

        template <typename owner_type>
        void insert_back(owner_type const* const object_ptr, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            m_invocationList.push_back(invocation_type::template from<owner_type>(object_ptr, method_ptr));
        }

        template <typename owner_type>
        void insert_back(owner_type& object, return_type(owner_type::* const method_ptr)(Args...))
        {
            m_invocationList.push_back(invocation_type::template from<owner_type>(object, method_ptr));
        }

        template <typename owner_type>
        void insert_back(owner_type const& object, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            m_invocationList.push_back(invocation_type::template from<owner_type>(object, method_ptr));
        }
#pragma endregion

#pragma region emplace_back
        template <typename owner_type>
        decltype(auto) emplace_back(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...))
        {
            return m_invocationList.emplace_back(object_ptr, method_ptr);
        }

        template <typename owner_type>
        decltype(auto) emplace_back(owner_type* const object_ptr, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            return m_invocationList.emplace_back(object_ptr, method_ptr);
        }

        template <typename owner_type>
        decltype(auto) emplace_back(owner_type& object, return_type(owner_type::* const method_ptr)(Args...))
        {
            return m_invocationList.emplace_back(object, method_ptr);
        }

        template <typename owner_type>
        decltype(auto) emplace_back(owner_type const& object, return_type(owner_type::* const method_ptr)(Args...) const)
        {
            return m_invocationList.emplace_back(object, method_ptr);
        }

        template <typename T CNDOXY(typename = typename std::enable_if_t<!std::is_same_v<invocation_type, typename std::decay_t<T>>>)>
        decltype(auto) emplace_back(T&& f)
        {
            return m_invocationList.emplace_back(f);
        }
#pragma endregion

        void pop_back() { m_invocationList.pop_back(); }

#pragma region resize
        void resize(size_type count) { m_invocationList.resize(count); }
        void resize(size_type count, const value_type& value) { m_invocationList.resize(count, value); }
#pragma endregion

#pragma region swap
        void swap(underlying_type& other) noexcept(noexcept(std::declval<underlying_type>().swap(std::declval<underlying_type>()))) { m_invocationList.swap(other); }
        void swap(multicast_delegate& other) noexcept(noexcept(std::declval<underlying_type>().swap(std::declval<underlying_type>()))) { m_invocationList.swap(other.m_invocationList); }
#pragma endregion

#pragma endregion

        explicit operator bool() const noexcept { return !empty(); }

        auto invoke_one(size_type index, Args... args) const
        {
            if constexpr (std::is_same_v<return_type, void>)
            {
                m_invocationList.at(index).invoke(std::forward<Args>(args)...);
            }
            else
            {
                return m_invocationList.at(index).invoke(std::forward<Args>(args)...);
            }
        }

        auto invoke(Args... args) const
        {
            if constexpr (std::is_same_v<return_type, void>)
            {
                for (auto& invocation : m_invocationList)
                    invocation.invoke(std::forward<Args>(args)...);
            }
            else
            {
                std::vector<return_type> ret;
                ret.reserve(m_invocationList.size());
                for (auto& invocation : m_invocationList)
                    ret.push_back(invocation.invoke(std::forward<Args>(args)...));
                return ret;
            }
        }

        auto operator()(Args... args) const
        {
            if constexpr (std::is_same_v<return_type, void>)
            {
                for (auto& invocation : m_invocationList)
                    invocation.invoke(std::forward<Args>(args)...);
            }
            else
            {
                std::vector<return_type> ret;
                ret.reserve(m_invocationList.size());
                for (auto& invocation : m_invocationList)
                    ret.push_back(invocation.invoke(std::forward<Args>(args)...));
                return ret;
            }
        }

    private:
        underlying_type m_invocationList;
    };
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template <typename return_type, typename ...Args>
    struct hash<legion::core::delegate<return_type(Args...)>>
    {
        size_t operator()(legion::core::delegate<return_type(Args...)> const& d) const noexcept
        {
            auto const seed(hash<void*>()(d.object_ptr_));

            return hash<typename legion::core::delegate<return_type(Args...)>::stub_ptr_type>()(
                d.stub_ptr_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}
#endif
