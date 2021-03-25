#include <core/engine/system.hpp>
#pragma once

namespace legion::core
{
    template<typename SelfType>
    template <void(SelfType::* func_type)(time::span), size_type charc>
    inline void System<SelfType>::createProcess(const char(&processChainName)[charc], time::span interval)
    {
        std::string name = std::string(processChainName) + nameOfType<SelfType>() + std::to_string(interval) + std::to_string(force_cast<intptr_t>(func_type)[0]);
        id_type id = nameHash(name);
        std::unique_ptr<schd::Process> process = std::make_unique<schd::Process>(name, id, interval);
        process->setOperation(delegate<void(time::span)>::from<SelfType, func_type>(reinterpret_cast<SelfType*>(this)));
        m_processes.emplace(id, std::move(process));

        schd::Scheduler::hookProcess<charc>(processChainName, pointer<schd::Process>{ m_processes[id].get() });
    }

}
