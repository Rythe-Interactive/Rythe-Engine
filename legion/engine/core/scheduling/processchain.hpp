#pragma once

#include <core/containers/pointer.hpp>
#include <core/containers/delegate.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/time/time.hpp>

#include <core/scheduling/process.hpp>

namespace legion::core::scheduling
{
    class ProcessChain
    {
    public:
        using chain_callback_type = void(time::span, time::span);
        using chain_callback_delegate = delegate<chain_callback_type>;
    private:
        std::string m_name;
        id_type m_nameHash = invalid_id;
        sparse_map<id_type, pointer<Process>> m_processes;

        multicast_delegate<chain_callback_type> m_onChainStart;
        multicast_delegate<chain_callback_type> m_onChainEnd;

    public:
        ProcessChain() = default;
        ProcessChain(ProcessChain&&) = default;
        ProcessChain& operator=(ProcessChain&&) = default;

        template<size_type charc>
        ProcessChain(const char(&name)[charc]);
        ProcessChain(cstring name);
        ProcessChain(cstring name, id_type id);

        /**@brief Returns the hash of the name of the process-chain.
         */
        id_type id();

        void subscribeToChainStart(const chain_callback_delegate& callback);
        void unsubscribeFromChainStart(const chain_callback_delegate& callback);

        void subscribeToChainEnd(const chain_callback_delegate& callback);
        void unsubscribeFromChainEnd(const chain_callback_delegate& callback);

        /**@brief Hook a process for execution with this chain.
         */
        void addProcess(Process& process);

        /**@brief Hook a process for execution with this chain.
         */
        void addProcess(pointer<Process> process);

        /**@brief Unhook a process from execution with this chain.
         */
        bool removeProcess(Process& process);

        /**@brief Unhook a process from execution with this chain.
         */
        bool removeProcess(pointer<Process> process);

        /**@brief Runs one iteration of the process-chains program loop without creating a new thread.
         * @note Loops through all hooked processes and executes them until they are all finished.
         */
        void runInCurrentThread(time::span deltaTime);
    };
}

#include <core/scheduling/processchain.inl>
