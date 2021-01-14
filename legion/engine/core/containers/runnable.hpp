#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>

namespace legion::core
{
    /**@class runnable
    * @brief A data structure encapsulating commands and jobs sent to any thread and it's parameters.
    */
    struct runnable_base
    {
        virtual void execute() LEGION_PURE;
    };

    template<typename Func>
    struct runnable : public runnable_base
    {
    protected:
        Func m_func;
    public:

        runnable() = default;
        runnable(const Func& func) : m_func(func) {}

        virtual void execute() override
        {
            m_func();
        }
    };
}
