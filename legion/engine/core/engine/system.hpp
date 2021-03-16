#pragma once
#include <memory>

#include <core/platform/platform.hpp>
#include <core/events/eventbus.hpp>
#include <core/time/time.hpp>
#include <core/ecs/registry.hpp>

namespace legion::core
{
    class SystemBase
    {
        friend class Engine;
    public:
        const id_type id;
        const std::string name;

        SystemBase(id_type id, const std::string& name) : id(id), name(name) {}

        virtual void setup() LEGION_PURE;

        virtual ~SystemBase() = default;
    };

    template<typename SelfType>
    class System : public SystemBase
    {
    public:
        System() : SystemBase(typeHash<SelfType>(), nameOfType<SelfType>()) {}
    };
}
