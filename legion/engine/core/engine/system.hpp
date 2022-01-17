#pragma once
#include <memory>

#include <core/platform/platform.hpp>
#include <core/events/event.hpp>
#include <core/time/time.hpp>
#include <core/scheduling/process.hpp>
#include <core/ecs/handles/entity.hpp>

namespace legion::core
{
    class Module;

    class SystemBase
    {
        friend class Engine;
    public:
        const id_type id;

        virtual ~SystemBase() = default;

    protected:
        std::unordered_map<id_type, std::unique_ptr<scheduling::Process>> m_processes;
        std::unordered_map<id_type, delegate<void(events::event_base&)>> m_bindings;

        SystemBase(id_type&& id) : id(id) {}

        // TODO: Inline all the things

        void destroyProcess(id_type procId);

        template <typename event_type CNDOXY(typename = inherits_from<event_type, events::event<event_type>>)>
        void unbindFromEvent(id_type bindingId);

        /**@brief Creates empty entity with the world as its parent.
         */
        L_NODISCARD static ecs::entity createEntity();

        L_NODISCARD static ecs::entity createEntity(const std::string& name);

        /**@brief Creates empty entity with a specific entity as its parent.
         * @param parent Entity to assign as the parent of the new entity.
         */
        L_NODISCARD static ecs::entity createEntity(ecs::entity parent);

        L_NODISCARD static ecs::entity createEntity(ecs::entity parent, const std::string& name);
        L_NODISCARD static ecs::entity createEntity(const std::string& name, ecs::entity parent);

        /**@brief Creates empty entity with a specific entity as its parent. Entity is serialized from a prototype.
         *        This function will also create any components or child entities in the prototype structure.
         * @param parent Entity to assign as the parent of the new entity.
         * @param prototype Prototype to serialize entity from.
         */
        //L_NODISCARD static ecs::entity createEntity(ecs::entity parent, const serialization::entity_prototype& prototype);

        /**@brief Creates empty entity with the world as its parent. Entity is serialized from a prototype.
         *        This function will also create any components or child entities in the prototype structure.
         * @param prototype Prototype to serialize entity from.
         */
       // L_NODISCARD static ecs::entity createEntity(const serialization::entity_prototype& prototype);

        /**@brief Insert event into bus and notify all subscribers.
         * @tparam event_type Event type to raise.
         * @param arguments Arguments to pass to the constructor of the event.
         */
        template<typename event_type, typename... Args CNDOXY(typename = inherits_from<event_type, events::event<event_type>>)>
        static void raiseEvent(Args&&... arguments);

        /**@brief Non-templated raise event function. Inserts event into bus and notifies all subscribers.
         * @param value Reference to the event to insert into the bus.
         */
        static void raiseEvent(events::event_base& value);

        /**@brief Unsafe, non-templated raise event function. This version is unsafe because it is allowed to trigger undefined behavior if the id is incompatible with the passed value.
         * @param value Reference to the event to insert into the bus.
         * @param id Type id of the event to invoke for. Overrides the polymorphic id of the reference passed as value.
         */
        static void raiseEventUnsafe(events::event_base& value, id_type id);
    };

    template<typename SelfType>
    class System : public SystemBase
    {
        friend class legion::core::Module;
    protected:
        template <void(SelfType::* func_type)(time::span), size_type charc>
        id_type createProcess(const char(&processChainName)[charc], time::span interval = 0);

        /**@brief Link a function to an event type in order to get notified whenever one gets raised.
         * @tparam event_type Event type to subscribe to.
         * @tparam func_type Function to bind to the event.
         */
        template <typename event_type, void(SelfType::* func_type)(event_type&) CNDOXY(typename = inherits_from<event_type, events::event<event_type>>)>
        id_type bindToEvent();

        template<void(SelfType::* func_type)()>
        auto queueJobs(size_type count);

        template<void(SelfType::* func_type)(id_type)>
        auto queueJobs(size_type count);

        template<typename Func>
        static auto queueJobs(size_type count, Func&& func);

    public:
        System() : SystemBase(typeHash<SelfType>()) {}

        virtual ~System() = default;

    };
}
