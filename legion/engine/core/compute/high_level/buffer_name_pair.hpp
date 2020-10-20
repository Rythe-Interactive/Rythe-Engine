#pragma once
#include <string>
#include <vector>
#include <core/types/primitives.hpp>

namespace legion::core::compute
{
    enum class dir
    {
        in, out, inout
    };

    struct buffer_create_info
    {
        byte* ptr;
        size_t size;
        dir direction;
    };


    struct buffer_name_pair
    {
        
        buffer_create_info create_info;
        std::string name;
    };

    template <class T>
    buffer_name_pair make_bnp(std::vector<T>& container,std::string str,dir = dir::in)
    {
        
    }


    // Kernel vector_add = fs::view("assets://kernels/vadd_kernel.cl").load_as<Kernel>("vector_add");
    // auto result = vector_add(make_bnp(lhs_vector,"lhs"),make_bnp(rhs_vector,"rhs"),make_bnp(result_vector,"res",dir::out));


}
