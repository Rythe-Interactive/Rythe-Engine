#pragma once
#include <memory>

#include <core/platform/platform.hpp>
#include <core/events/eventbus.hpp>
#include <core/time/time.hpp>
#include <core/ecs/registry.hpp>
#include <core/common/hash.hpp>

namespace legion::core
{
    class SystemBase
    {
        friend class Engine;
    public:
        const type_reference id;

    protected:
        SystemBase(type_reference&& id) : id(id) {}
    };

    template<typename SelfType>
    class System : public SystemBase
    {
    public:
        System() : SystemBase(make_hash<SelfType>()) {}


    };
}
