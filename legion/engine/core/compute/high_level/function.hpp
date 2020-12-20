#pragma once
#include <type_traits>
#include <variant>
#include <tuple>
#include <array>

#include <core/common/result.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/compute/buffer.hpp>
#include <core/compute/kernel.hpp>
#include <core/compute/program.hpp>
#include <core/detail/internals.hpp>
#include <core/filesystem/resource.hpp>

#include <Optick/optick.h>

namespace legion::core::compute {

    struct in_ident {};
    struct out_ident {};
    struct inout_ident {};

    namespace detail {
        struct buffer_base
        {
            buffer_base(byte* buffer, size_type size, std::string n) : container(std::make_pair(buffer, size)), name(std::move(n)) {}
            buffer_base(const buffer_base& other) = default;
            buffer_base(buffer_base&& other) noexcept = default;
            buffer_base& operator=(const buffer_base& other) = default;
            buffer_base& operator=(buffer_base&& other) noexcept = default;
            ~buffer_base() = default;

            std::pair<byte*, size_type> container;
            std::string name;
        };
    }

    struct invalid_karg_type {};

    struct karg
    {

        karg(invalid_karg_type){}
        template <class T, std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, karg>, int > = 0>
        karg(T& v, const std::string& n = "") : container(&v, sizeof(T)), name(n){}
        karg(const karg&) = default;
        karg(karg&&) noexcept = default;
        karg& operator=(const karg&) = default;
        karg& operator=(karg&&) noexcept = default;
        ~karg() = default;

        std::pair<void*, size_type> container;
        std::string name;
    };



    /**
     * @class in
     * @brief Wraps a vector parameter to a kernel invocation, marks it
     * as an in-bound parameter, can have an optional name.
     */
    template <class T>
    struct in : public in_ident, public detail::buffer_base
    {
        static_assert(is_vector<std::remove_reference_t<T>>::value, "T needs to be a vector");

        in(T& vec, std::string name = "") :
            buffer_base(reinterpret_cast<byte*>(vec.data()),
                sizeof(typename std::remove_reference_t<T>::value_type)* vec.size(), name) {}
        ~in() = default;
        in(const in& other) = default;
        in(in&& other) noexcept = default;
        in& operator=(const in& other) = default;
        in& operator=(in&& other) noexcept = default;
        using value_type = T;
    };

    /**
     * @class out
     * @brief Wraps a vector parameter to a kernel invocation, marks it
     * as an out-bound parameter, can have an optional name.
     */
    template <class T>
    struct out : public out_ident, public detail::buffer_base
    {
        static_assert(is_vector<std::remove_reference_t<T>>::value, "T needs to be a vector");

        out(T& vec, std::string name = "") :
            buffer_base(reinterpret_cast<byte*>(vec.data()),
                sizeof(typename std::remove_reference_t<T>::value_type)* vec.size(), name) {}
        ~out() = default;
        out(const out& other) = default;
        out(out&& other) noexcept = default;
        out& operator=(const out& other) = default;
        out& operator=(out&& other) noexcept = default;
        using value_type = T;
    };

    /**
     * @class in
     * @brief Wraps a vector parameter to a kernel invocation, marks it
     * as an in and out-bound parameter, can have an optional name.
     */
    template <class T>
    struct inout : public inout_ident, public detail::buffer_base
    {
        static_assert(is_vector<std::remove_reference_t<T>>::value, "T needs to be a vector");

        inout(T& vec, std::string name = "") :
            buffer_base(reinterpret_cast<byte*>(vec.data()),
                sizeof(typename std::remove_reference_t<T>::value_type)* vec.size(), name) {}
        ~inout() = default;
        inout(const inout& other) = default;
        inout(inout&& other) noexcept = default;
        inout& operator=(const inout& other) = default;
        inout& operator=(inout&& other) noexcept = default;
        using value_type = T;
    };

    class function_base
    {

    protected:
        using invoke_buffer_container = std::vector<std::pair<detail::buffer_base*, buffer_type>>;
        using dvar = std::variant<std::tuple<size_type>,
            std::tuple<size_type, size_type>,
            std::tuple<size_type, size_type, size_type>
        >;

        //invokes the NdRangeKernel
        [[nodiscard]] common::result<void, void> invoke(dvar global, invoke_buffer_container& parameters, std::vector<karg> kernelArgs) const;
        [[nodiscard]] common::result<void, void> invoke2(dvar global, std::vector<Buffer> buffers, std::vector<karg> kernelArgs) const;


        std::shared_ptr<Kernel> m_kernel;
        std::shared_ptr<Program> m_program;
        size_t m_locals = 512;
    public:


        /**
         * @brief Sets how many work-elements should be processed concurrently.
         * @param locals Number of parallel processes or 0,
         *         when 0 the measured max will be used.
         * @return How many parallel processes are going to be used.
         */
        size_type setLocalSize(size_type locals)
        {
            OPTICK_EVENT();
            const size_type max = m_kernel->getMaxWorkSize();

            if (locals == 0)
            {
                m_locals = max;
            }
            else
                m_locals = (std::min)(locals, max);

            return m_locals;
        }
    };

    class function final : public function_base
    {
    public:
        using function_base::setLocalSize;
        function(std::string name) : m_name(std::move(name)) {}
        function() = default;
        function(function&& other) noexcept
        {
            m_program = std::move(other.m_program);
            m_kernel = std::move(other.m_kernel);
            m_locals = std::move(other.m_locals);
        }
        function(const function& other)
        {
            m_program = other.m_program;
            m_kernel = other.m_kernel;
            m_locals = other.m_locals;
        }
        function& operator=(const function& other)
        {
            m_program = other.m_program;
            m_kernel = other.m_kernel;
            m_locals = other.m_locals;
            return *this;
        }

        function& operator=(function&& other)
        {
            m_program = std::move(other.m_program);
            m_kernel = std::move(other.m_kernel);
            m_locals = std::move(other.m_locals);
            return *this;
        }
        /**
          * @brief Sets the program from which to create the wrapped kernel.
          */
        void setProgram(Program&& p)
        {
            m_program = std::make_shared<Program>(p);
            m_kernel = std::make_shared<Kernel>(m_program->kernelContext(m_name));
            m_locals = m_kernel->getMaxWorkSize();
        }

        /**
         * @brief Invokes the wrapped kernel with the passed buffers
         * @param dispatch_size How many items to process.
         * @param args a collection of either vectors and wrapped vectors or compute::Buffers
         * @return Ok() if the kernel succeeded or Err() otherwise
         */
        template <typename... Args>
        common::result<void, void> operator()(std::variant<size_type, math::ivec2, math::ivec3> dispatch_size, Args&&... args)
        {
            OPTICK_EVENT();
            dvar dim;

            if (std::holds_alternative<size_type>(dispatch_size))
            {
                dim = std::make_tuple(std::get<0>(dispatch_size));
            }
            else if (std::holds_alternative<math::ivec2>(dispatch_size))
            {
                dim = std::make_tuple(static_cast<size_type>(std::get<1>(dispatch_size)[0]),
                    static_cast<size_type>(std::get<1>(dispatch_size)[1]));
            }
            else if (std::holds_alternative<math::ivec3>(dispatch_size))
            {
                dim = std::make_tuple(static_cast<size_type>(std::get<2>(dispatch_size)[0]),
                    static_cast<size_type>(std::get<2>(dispatch_size)[1]),
                    static_cast<size_type>(std::get<2>(dispatch_size)[2]));
            }

            //check if we are dealing with a list of buffers or a list of vectors
            //TODO(algo-ryth-mix) Update the cppcheck version of the CI once this bug is resolved!
            //cppcheck has an issue with if contexpr and the || in here
            //cppcheck-suppress internalAstError
            if constexpr (((std::is_same_v<compute::Buffer, std::remove_reference_t<Args>> || std::is_same_v<karg, Args>) && ...))
            {
                return invoke_helper_buffers(dim, std::forward<Args>(args)...);
            }
            else
            {
                return invoke_helper_raw(dim, std::forward<Args>(args)...);
            }
        }


        static void from_resource(function* value, const filesystem::basic_resource& resource)
        {
            value->setProgram(resource.to<Program>());
        }

        bool isValid() const
        {
            return m_program != nullptr;
        }

    private:
        std::string m_name;

        //transformation from in / out / inout to pair(buffer,"in") / pair(buffer,"out") / pair(buffer,"inout")
        template <class T>
        static std::pair<detail::buffer_base*, buffer_type>  transform_to_pairs(T& buffer_container)
        {
            OPTICK_EVENT();
            using detail::buffer_base;

            if constexpr (std::is_base_of_v<in_ident, T>)
            {
                return std::pair<buffer_base*, buffer_type>(static_cast<buffer_base*>(&buffer_container), buffer_type::READ_BUFFER);
            }
            else if constexpr (std::is_base_of_v<out_ident, T>)
            {
                return std::pair<buffer_base*, buffer_type>(static_cast<buffer_base*>(&buffer_container), buffer_type::WRITE_BUFFER);
            }
            else if constexpr (std::is_base_of_v<inout_ident, T>)
            {
                return std::pair<buffer_base*, buffer_type>(static_cast<buffer_base*>(&buffer_container), buffer_type::READ_BUFFER | buffer_type::WRITE_BUFFER);
            }
            else
                return std::pair<buffer_base*, buffer_type>(nullptr, buffer_type::WRITE_BUFFER);
        }

        template <class T>
        static karg transform_to_karg(T& buffer_container)
        {
            OPTICK_EVENT();
            if constexpr (std::is_same_v<karg, std::remove_reference_t<T>>)
                return buffer_container;
            else
            {
                return karg(invalid_karg_type{});
            }

        }

        template <class T>
        static Buffer transform_to_buffer(T& buffer_container)
        {
            OPTICK_EVENT();
            if constexpr (std::is_same_v<Buffer, T>)
                return buffer_container;
            else return Buffer(nullptr, nullptr, 0, buffer_type::WRITE_BUFFER, "broken buffer");
        }


    private:
        template <typename... Args>
        common::result<void, void> invoke_helper_raw(dvar dispatch_size, Args&& ... args)
        {
            OPTICK_EVENT();
            //do some sanity checking args either need to be in(vector) out(vector) inout(vector) vector or karg
            static_assert(((
                std::is_same_v<karg, Args> ||
                std::is_base_of_v<detail::buffer_base, Args> ||
                is_vector<std::remove_reference_t<Args>>::value) && ...), "Types passed to operator() must be vector or in,out,inout");


            //promote vector to in(vector) leave the rest alone
            std::tuple container = { std::conditional_t<is_vector<std::remove_reference_t<Args>>::value,in<Args>,Args>(args)... };

            auto kargs = std::apply(
                [](auto&& ... x)
                {
                    return std::vector<karg>{function::transform_to_karg(x)...};
                }, container);

            //transform from tuple(in,out,inout,...) to vector(pair(buffer,"in"),pair(buffer,"out"), ...)
            auto vector = std::apply(
                [](auto&&...x)
                {
                    return invoke_buffer_container{ function::transform_to_pairs(x)... };
                }, container);


            //we finally transformed it into a way that the non-templated function can use
            return invoke(dispatch_size, vector, kargs);
        }

        template <typename... Args>
        common::result<void, void> invoke_helper_buffers(dvar dispatch_size, Args&&... args)
        {
            OPTICK_EVENT();
            static_assert(((std::is_same_v<compute::Buffer, std::remove_reference_t<Args>> || std::is_same_v<karg, std::remove_reference_t<Args>> ) && ...),
                "Types passed to operator must be Buffer");

            std::tuple tpl = { args... };

            auto kargs = std::apply(
                [](auto&& ... x)
                {
                    return std::vector<karg>{function::transform_to_karg(x)...};
                }, tpl);
            auto buffers = std::apply(
                [](auto&&...x)
                {
                    return std::vector<Buffer>{ function::transform_to_buffer(x)... };
                }, tpl);

            return invoke2(dispatch_size, buffers, kargs);
        }
    };
}
