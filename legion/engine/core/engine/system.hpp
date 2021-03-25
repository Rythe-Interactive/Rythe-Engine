#pragma once
#include <memory>

#include <core/platform/platform.hpp>
#include <core/events/eventbus.hpp>
#include <core/time/time.hpp>
#include <core/ecs/registry.hpp>
#include <core/common/hash.hpp>
#include <core/scheduling/scheduling.hpp>

namespace legion::core
{
    class Module;

    class SystemBase
    {
        friend class Engine;
    public:
        const type_reference id;

    protected:
        std::unordered_map<id_type, std::unique_ptr<schd::Process>> m_processes;

        SystemBase(type_reference&& id) : id(id) {}
    };

    template<typename SelfType>
    class System : public SystemBase
    {
        friend class legion::core::Module;
    protected:
        template <void(SelfType::* func_type)(time::span), size_type charc>
        void createProcess(const char(&processChainName)[charc], time::span interval = 0);

    public:
        System() : SystemBase(make_hash<SelfType>()) {}


    };
}

#include <core/engine/system.inl>
