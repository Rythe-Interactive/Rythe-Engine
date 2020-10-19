#pragma once
#include <type_traits>
#include <core/common/result.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/compute/buffer.hpp>
#include <core/compute/kernel.hpp>
#include <core/compute/program.hpp>
#include <core/filesystem/resource.hpp>

namespace args::core::compute {

    struct in_ident {};
    struct out_ident {};
    struct inout_ident {};

    struct buffer_base
    {
        buffer_base(byte* buffer, size_t size, std::string n) : container(std::make_pair(buffer, size)), name(std::move(n)) {}
        buffer_base(const buffer_base& other) = default;
        buffer_base(buffer_base&& other) noexcept = default;
        buffer_base& operator=(const buffer_base& other) = default;
        buffer_base& operator=(buffer_base&& other) noexcept = default;
        ~buffer_base() = default;

        std::pair<byte*, size_t> container;
        std::string name;
    };


    template <class T>
    struct in : public in_ident, public buffer_base
    {
        static_assert(is_vector<std::remove_reference_t<T>>::value,"T needs to be a vector");

        in(T& vec, std::string name = "") : buffer_base(reinterpret_cast<byte*>(vec.data()), sizeof(std::remove_reference_t<T>::value_type)* vec.size(), name) {}
        ~in() = default;
        in(const in& other) = default;
        in(in&& other) noexcept = default;
        in& operator=(const in& other) = default;
        in& operator=(in&& other) noexcept = default;
        using value_type = T;
    };

    template <class T>
    struct out : public out_ident, public buffer_base
    {
        static_assert(is_vector<std::remove_reference_t<T>>::value,"T needs to be a vector");

        out(T& vec, std::string name = "") : buffer_base(reinterpret_cast<byte*>(vec.data()), sizeof(std::remove_reference_t<T>::value_type)* vec.size(), name) {}
        ~out() = default;
        out(const out& other) = default;
        out(out&& other) noexcept = default;
        out& operator=(const out& other) = default;
        out& operator=(out&& other) noexcept = default;
        using value_type = T;
    };

    template <class T>
    struct inout : public inout_ident, public buffer_base
    {
        static_assert(is_vector<std::remove_reference_t<T>>::value,"T needs to be a vector");

        inout(T& vec, std::string name = "") : buffer_base(reinterpret_cast<byte*>(vec.data()), sizeof(T::value_type)* vec.size(), name) {}
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
        common::result<void,void> invoke(size_type global, std::vector<std::pair<buffer_base*,buffer_type>>& parameters) const;
        std::unique_ptr<Kernel> m_kernel;
        std::unique_ptr<Program> m_program;

    };

    class function : public function_base
    {
    public:

        function(std::string name) : m_name(std::move(name)){}

        void setProgram(Program&& p)
        {
            m_program = std::make_unique<Program>(p);
            m_kernel = std::make_unique<Kernel>(m_program->kernelContext(m_name));
        }

        template <typename... Args>
        common::result<void, void> operator()(size_type dispatch_size,Args&& ... args)
        {

            //do some sanity checking args either need to be in(vector) out(vector) inout(vector) or vector
            static_assert(((
                std::is_base_of_v<buffer_base, Args> ||
                is_vector<std::remove_reference_t<Args>>::value) && ...), "Type passed to operator() must be vector or in,out,inout");


            //promote vector to in(vector) leave the rest alone
            std::tuple container = { std::conditional_t<is_vector<std::remove_reference_t<Args>>::value,in<Args>,Args>(args)... };

            //transform from tuple(in,out,inout,...) to vector(pair(buffer,"in"),pair(buffer,"out"), ...)
            auto vector = std::apply(
                [](auto...x)
                {
                    return std::vector<std::pair<buffer_base*,buffer_type>> { function::transform_to_pairs(x)... };
                },container);


            //we finally transformed it into a way that the non-templated function can use
            return invoke(dispatch_size, vector);
        }

        static void from_resource(function* value, const filesystem::basic_resource& resource)
        {
            value->setProgram(resource.to<Program>());
        }

    private:
        std::string m_name;

        //transformation from in / out / inout to pair(buffer,"in") / pair(buffer,"out") / pair(buffer,"inout")
        template <class T>
        static std::pair<buffer_base*,buffer_type>  transform_to_pairs(T& buffer_container)
        {
            if constexpr (std::is_base_of_v<in_ident,T>)
            {
                return std::pair<buffer_base*,buffer_type>(static_cast<buffer_base*>(&buffer_container),buffer_type::READ_BUFFER);    
            }
            else if constexpr (std::is_base_of_v<out_ident,T>)
            {
                return std::pair<buffer_base*,buffer_type>(static_cast<buffer_base*>(&buffer_container),buffer_type::WRITE_BUFFER);
            }
            else
            {
                return std::pair<buffer_base*,buffer_type>(static_cast<buffer_base*>(&buffer_container),buffer_type::READ_BUFFER | buffer_type::WRITE_BUFFER);
            }
        }
    };
}

// 
// function vector_add = fs::view("path_to_kernel.cl").load_as<function>("vector_add");
// result = vector_add(a,b,out(ret));
// 0~~~~~~~~~~~~~~~~~~~^ ^ ^
//
//