#pragma once
/**
 * Copyright 2020 Raphael Baier, The Args Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <tuple>
#include <type_traits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <functional>

#include <core/platform/platform.hpp>

namespace legion::core::common {


    class result_ident {};

    template <class... T>
    class result;

    class ok_ident {};
    class err_ident {};
    struct tuple_create_helper {};
    template<class... Stuff> struct many_t {};

    using empty_t = many_t<>;

    template <class T, class Original>   struct try_static_cast_result;
    template <class Original>           struct try_static_cast_result<void, Original> { using type = Original; };
    template <class T, class Original>   struct try_static_cast_result { using type = T; };

    template <class T, class Original>
    auto try_static_cast(const Original& o)
    {
        if constexpr (std::is_same<T, void>::value) return o;
        else return static_cast<T>(o);
    }

    /********************************************************************************/
    /**@brief Ok template defs*/
    template<class... Any>
    class ok_proxy;

    template <>                         class ok_proxy<void> : public ok_ident
    {
    public:
        operator ok_proxy<>() const;
    };
    template <>                         class ok_proxy<> : public ok_ident
    {
        operator ok_proxy<void>()
        {
            return ok_proxy<void>();
        }
    };
    template <class T>                  class ok_proxy<T> : public ok_ident
    {
    public:
        ok_proxy(ok_proxy&&) noexcept(noexcept(T(std::declval<T>()))) = default;
        ok_proxy(T val) : m_val(std::move(val)) {}
        explicit ok_proxy(const std::tuple<T>& tpl) : m_val(std::get<0>(tpl)) {}
        explicit ok_proxy(tuple_create_helper, std::tuple<T>& tpl) : m_val(std::get<0>(tpl)) {}

        operator T& () {
            return m_val;
        }
        operator const T& () const {
            return m_val;
        }
    private:
        T m_val;

    };
    template <class T, class... Any>    class ok_proxy<T, Any...> : public ok_ident
    {
    public:
        using tuple_type = std::tuple<T, Any...>;
        ok_proxy(ok_proxy&&) noexcept(noexcept(tuple_type(std::declval<tuple_type>()))) = default;

        template <typename = std::enable_if_t<!std::is_same<T, tuple_create_helper>::value>>
        ok_proxy(T val, Any... args) : ok_proxy(tuple_create_helper{}, std::make_tuple(std::move(val), std::move(args)...)) {}
        explicit ok_proxy(tuple_type&& tpl) :m_values(std::move(tpl)) {}
        explicit ok_proxy(tuple_create_helper, tuple_type&& tpl) :m_values(std::move(tpl)) {}

        operator std::tuple<T, Any ...>()  const
        {
            return std::move(m_values);
        }

        operator const std::tuple<T, Any...>& () const
        {
            return m_values;
        }
    private:
        tuple_type m_values;

    };

    inline ok_proxy<void>::operator ok_proxy<>() const {
        return ok_proxy<>();
    }

    inline ok_proxy<void> Ok()
    {
        return ok_proxy<void>{};
    }


    template <class T, class...Any CNDOXY(std::enable_if_t<!(std::is_base_of_v<result_ident, T> && sizeof...(Any) == 0), int> = 0)>
    inline ok_proxy<T, Any...> Ok(T&& t, Any&& ... any)
    {
        return ok_proxy<T, Any...>(std::move(t), std::forward<Any>(any)...);
    }

    template <class T, class...Any CNDOXY(std::enable_if_t<!(std::is_base_of_v<result_ident, T> && sizeof...(Any) == 0), int> = 0)>
    inline ok_proxy<T, Any...> Ok(T& t, Any&& ... any)
    {
        return ok_proxy<T, Any...>(t, std::forward<Any>(any)...);
    }

    template<class... Args>
    inline ok_proxy<Args...> Ok(std::tuple<Args...> args)
    {
        return std::apply(Ok, args);
    }

    template <class... Args>
    inline typename result<Args...>::ok_type Ok_of(result<Args...>& res)
    {
        return Ok(res.get());
    }
    /********************************************************************************/


    /********************************************************************************/
    /**@brief Err template defs*/
    template <class... Any>
    class err_proxy;

    template <>                         class err_proxy<void> : public err_ident
    {
    public:
        err_proxy() {}
        operator err_proxy<>() const;
    };
    template <>                         class err_proxy<> : public err_ident
    {
        operator err_proxy<void>()
        {
            return err_proxy<void>();
        }
    };
    template <class T>                  class err_proxy<T> : public err_ident
    {
    public:
        err_proxy(err_proxy&&) noexcept(noexcept(T(std::declval<T>()))) = default;
        err_proxy(T  val) : m_val(std::move(val)) {}
        explicit err_proxy(const std::tuple<T >& tpl) : m_val(std::get<0>(tpl)) {}
        explicit err_proxy(tuple_create_helper, std::tuple<T >& tpl) : m_val(std::get<0>(tpl)) {}

        operator T& () {
            return m_val;
        }
        operator const T& () const {
            return m_val;
        }
    private:
        T  m_val;
    };
    template <class T, class... Any>    class err_proxy<T, Any...> : public err_ident
    {
    public:
        using tuple_type = std::tuple<T, Any ...>;
        template <typename = std::enable_if_t<!std::is_same<T, tuple_create_helper>::value>>
        err_proxy(T  val, Any ... args) : err_proxy(tuple_create_helper{}, std::make_tuple(std::move(val), std::move(args...))) {}
        explicit err_proxy(tuple_type&& tpl) : m_values(std::move(tpl)) {}
        explicit err_proxy(tuple_create_helper, tuple_type&& tpl) :m_values(std::move(tpl)) {}

        operator std::tuple<T, Any ...>& ()
        {
            return m_values;
        }
        operator const std::tuple<T, Any ...>& () const
        {
            return m_values;
        }
    private:
        tuple_type m_values;
    };

    inline err_proxy<void>::operator err_proxy<>() const {
        return err_proxy<>();
    }

    inline err_proxy<void> Err()
    {
        return err_proxy<void>{};
    }

    template <class T, class...Any CNDOXY(std::enable_if_t<!(std::is_base_of_v<result_ident, std::remove_reference<T>> && sizeof...(Any) == 0), int> = 0)>
    inline err_proxy<T, Any...> Err(T&& t, Any&& ... any)
    {
        return err_proxy<T, Any...>(std::move(t), std::forward<Any>(any)...);
    }

    template <class T, class...Any CNDOXY(std::enable_if_t<!(std::is_base_of_v<result_ident, std::remove_reference<T>> && sizeof...(Any) == 0), int> = 0)>
    inline err_proxy<T, Any...> Err(T& t, Any&& ... any)
    {
        return err_proxy<T, Any...>(t, std::forward<Any>(any)...);
    }

    template<class... Args>
    inline err_proxy<Args...> Err(std::tuple<Args...> args)
    {
        return std::apply(Err, args);
    }

    template <class... Args>
    inline typename result<Args...>::err_type Err_of(result<Args...>& res)
    {
        return Err(res.get_error());
    }

    /********************************************************************************/




    template<class... Lots>
    class result_impl;

    template <class OkType, class ErrType, class OkResultType, class ErrResultType>
    class result_impl<OkType, ErrType, OkResultType, ErrResultType>
    {
    public:
        using err_type = ErrType;
        using ok_type = OkType;
        using err_result_t = ErrResultType;
        using ok_result_t = OkResultType;

        result_impl(std::unique_ptr<ok_type>  ok, std::unique_ptr<err_type>  err) :
            m_err(std::move(err)), m_ok(std::move(ok)) {}
        result_impl(const result_impl&) = delete;
        result_impl(result_impl&&) noexcept = default;
        result_impl& operator=(const result_impl&) = delete;
        result_impl& operator=(result_impl&&) noexcept = default;

        virtual ~result_impl() = default;

        typename try_static_cast_result<ok_result_t, ok_type>::type  get()
        {
            if (m_ok) return try_static_cast<ok_result_t>(std::move(*m_ok.get()));
            else if (m_err) throw try_static_cast<err_result_t>(*m_err);
            else throw std::runtime_error("both ok and err were empty!");
        }

        operator typename try_static_cast_result<ok_result_t, ok_type>::type() {
            return get();
        }
        operator typename try_static_cast_result<ok_result_t, ok_type>::type() const {
            return get();
        }

        template <class Func,class... Args>
        auto except(Func&& f,Args&&... args) -> decltype(auto)
        {
            if(has_err())
            {
                return std::invoke(f,get_error(),std::forward<Args>(args)...);
            }
            return get();
        }
        template <class Func,class... Args>
        auto except(const Func& f,Args&&... args) -> decltype(auto)
        {
            if(has_err())
            {
                return std::invoke(f,get_error(),std::forward<Args>(args)...);
            }
            return get();
        }

        L_NODISCARD bool valid() const noexcept
        {
            return m_ok != nullptr;
        }
        bool has_err() noexcept
        {
            return m_err != nullptr && m_ok == nullptr;
        }

        L_NODISCARD const typename try_static_cast_result<err_result_t, err_type>::type& get_error() const
        {
            if (m_err) return try_static_cast<err_result_t>(*m_err);
            throw std::runtime_error("this result would have been valid!");
        }

        L_NODISCARD typename try_static_cast_result<err_result_t, err_type>::type get_error()
        {
            if (m_err) return try_static_cast<err_result_t>(*m_err);
            throw std::runtime_error("this result would have been valid!");
        }

        L_NORETURN void rethrow()
        {
            throw try_static_cast<err_result_t>(*m_err);
        }
        void maybe_rethrow()
        {
            if (has_err()) rethrow();
        }

    protected:
        std::unique_ptr<err_type> m_err;
        std::unique_ptr<ok_type> m_ok;
    };


    template <class ErrType, class OkResultType, class ErrResultType>
    class result_impl<void, ErrType, OkResultType, ErrResultType>
    {
    public:
        using err_type = ErrType;
        using ok_type = void;
        using err_result_t = ErrResultType;
        using ok_result_t = OkResultType;

        result_impl(std::unique_ptr<ok_type>  ok, std::unique_ptr<err_type>  err) :
            m_err(std::move(err)), m_ok(std::move(ok)) {}
        result_impl(const result_impl&) = delete;
        result_impl(result_impl&&) noexcept = default;
        result_impl& operator=(const result_impl&) = delete;
        result_impl& operator=(result_impl&&) noexcept = default;

        virtual ~result_impl() = default;

        template <class Func, class... Args>
        void except(Func && f, Args&&... args)
        {
            if (has_err())
            {
                return std::invoke(f, get_error(), std::forward<Args>(args)...);
            }
        }
        template <class Func, class... Args>
        void except(const Func & f, Args&&... args)
        {
            if (has_err())
            {
                return std::invoke(f, get_error(), std::forward<Args>(args)...);
            }
        }

        L_NODISCARD bool valid() const noexcept
        {
            return m_ok != nullptr;
        }
        bool has_err() noexcept
        {
            return m_err != nullptr && m_ok == nullptr;
        }

        L_NODISCARD const typename try_static_cast_result<err_result_t, err_type>::type& get_error() const
        {
            if (m_err) return try_static_cast<err_result_t>(*m_err);
            throw std::runtime_error("this result would have been valid!");
        }

        L_NODISCARD typename try_static_cast_result<err_result_t, err_type>::type get_error()
        {
            if (m_err) return try_static_cast<err_result_t>(*m_err);
            throw std::runtime_error("this result would have been valid!");
        }

        L_NORETURN void rethrow()
        {
            throw try_static_cast<err_result_t>(*m_err);
        }
        void maybe_rethrow()
        {
            if (has_err()) rethrow();
        }

    protected:
        std::unique_ptr<err_type> m_err;
        std::unique_ptr<ok_type> m_ok;
    };

    template <class... OkArgs, class... ErrArgs>
    class result<many_t<OkArgs...>, many_t<ErrArgs...>> :
        public result_impl<ok_proxy<OkArgs...>, err_proxy<ErrArgs...>, std::tuple<OkArgs...>, std::tuple<ErrArgs...>>,
        public result_ident {
    public:
        using rimpl = result_impl<ok_proxy<OkArgs...>, err_proxy<ErrArgs...>, std::tuple<OkArgs...>, std::tuple<ErrArgs...>>;
        result(ok_proxy<OkArgs...>&& ok) : rimpl(std::unique_ptr<ok_proxy<OkArgs...>>(new ok_proxy<OkArgs...>(std::move(ok))), nullptr) {};
        result(err_proxy<ErrArgs...>&& err) : rimpl(nullptr, std::unique_ptr<err_proxy<ErrArgs...>>(new err_proxy<ErrArgs...>(std::move(err)))) {};
        using rimpl::operator typename try_static_cast_result<std::tuple<OkArgs...>, ok_proxy<OkArgs...>>::type;
    };
    template <class ErrType, class... Args>
    class result<many_t<Args...>, ErrType> :
        public result_impl<ok_proxy<Args...>, err_proxy<ErrType>, std::tuple<Args...>, ErrType>,
        public result_ident {
    public:
        using rimpl = result_impl<ok_proxy<Args...>, err_proxy<ErrType>, std::tuple<Args...>, ErrType>;
        result(ok_proxy<Args...>&& ok) : rimpl(std::unique_ptr<ok_proxy<Args...>>(new ok_proxy<Args...>(std::move(ok))), nullptr) {};
        result(err_proxy<ErrType>&& err) : rimpl(nullptr, std::unique_ptr<err_proxy<ErrType>>(new err_proxy<ErrType>(std::move(err)))) {};
        using rimpl::operator typename try_static_cast_result<std::tuple<Args...>, ok_proxy<Args...>>::type;
    };
    template <class OkType, class... Args>
    class result<OkType, many_t<Args...>> :
        public result_impl<ok_proxy<OkType>, err_proxy<Args...>, OkType, std::tuple<Args...>>,
        public result_ident {
    public:
        using rimpl = result_impl<ok_proxy<OkType>, err_proxy<Args...>, OkType, std::tuple<Args...>>;
        result(ok_proxy<OkType>&& ok) : rimpl(std::unique_ptr<ok_proxy<OkType>>(new ok_proxy<OkType>(std::move(ok))), nullptr) {};
        result(err_proxy<Args...>&& err) : rimpl(nullptr, std::unique_ptr<err_proxy<Args...>>(new err_proxy<Args...>(std::move(err)))) {};
        using rimpl::operator typename try_static_cast_result<OkType, ok_proxy<OkType>>::type;
    };
    template <class OkType, class ErrType>
    class result<OkType, ErrType> :
        public result_impl<ok_proxy<OkType>, err_proxy<ErrType>, OkType, ErrType>,
        public result_ident {
    public:
        using rimpl = result_impl<ok_proxy<OkType>, err_proxy<ErrType>, OkType, ErrType>;
        result(ok_proxy<OkType>&& ok) : rimpl(std::unique_ptr<ok_proxy<OkType>>(new ok_proxy<OkType>(std::move(ok))), nullptr) {};
        result(err_proxy<ErrType>&& err) : rimpl(nullptr, std::unique_ptr<err_proxy<ErrType>>(new err_proxy<ErrType>(std::move(err)))) {};
        using rimpl::operator typename try_static_cast_result<OkType, ok_proxy<OkType>>::type;
    };


    class valid_t {};

    template <class Result>
    class result_decay
    {
    public:
        using ok_type = typename Result::ok_result_t;
        using err_type = typename Result::err_result_t;

        result_decay(Result r) : m_r{ std::move(r) } {}

        bool operator==(valid_t)
        {
            return m_r.valid();
        }
        bool operator!=(valid_t)
        {
            return m_r.has_err();
        }
        bool operator==(std::nullptr_t)
        {
            return m_r.has_err();
        }
        bool operator!=(std::nullptr_t)
        {
            return m_r.valid();
        }
        operator ok_type ()
        {
            return m_r.get();
        }
        operator Result ()
        {
            return m_r;
        }

        auto decay() -> decltype(auto) 
        {
            return m_r.get();
        }

        template <class Func,class... Args>
        auto except(Func&& f,Args&&... args) -> decltype(auto)
        {
            return m_r.except(std::forward<Func>(f),std::forward<Args>(args)...);
        }

        template <class Func,class... Args>
        auto except(const Func& f,Args&&... args) -> decltype(auto)
        {
            return m_r.except(f,std::forward<Args>(args)...);
        }


        err_type get_error()
        {
            return m_r.get_error();
        }


    private:
        Result m_r;

    };

   

    /**@brief convenience wrapper around result_decay that does not need the
     *        common::result<...>
     **/
    template <class...Args>
    using result_decay_more = result_decay<result<Args...>>;

    template <class T,class E>
    auto decay(result_decay_more<T,E>& x) ->decltype(auto) { return x.get(); }

    constexpr valid_t valid{};

}
