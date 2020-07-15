#pragma once
#include <cstdint> //linux std::(u)int(8-64)_t
#include <cstddef> //msc   std::(u)int(8-64)_t 
#include <vector>  //      std::vector<T>


namespace args::core
{
    using byte_t = std::uint8_t;
    using byte_vec = std::vector<byte_t>;
}
