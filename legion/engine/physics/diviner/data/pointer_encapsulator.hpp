#pragma once

namespace legion::physics
{
    template<class T>
    class PointerEncapsulator
    {
    public:

        T* ptr = nullptr;
    };
}
