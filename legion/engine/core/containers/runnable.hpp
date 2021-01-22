#pragma once
#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <memory>
#include <functional>

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
        std::shared_ptr<std::remove_reference_t<Func>> m_func;
    public:

        runnable() = default;
        runnable(const Func& func) : m_func(new Func(func)) {}

        virtual void execute() override
        {
            std::invoke(*m_func);
        }
    };
}
