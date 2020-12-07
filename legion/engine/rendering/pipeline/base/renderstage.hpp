#pragma once
#include <application/application.hpp>
#include <rendering/components/camera.hpp>

namespace legion::rendering
{
    class RenderPipelineBase;

    class RenderStageBase
    {
        friend class Renderer;
    protected:
        static ecs::EcsRegistry* m_ecs;
        static schd::Scheduler* m_scheduler;
        static events::EventBus* m_eventBus;
    public:
        static RenderPipelineBase* m_pipeline;

        virtual void setup(app::window& context) LEGION_PURE;
        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) LEGION_PURE;
        virtual priority_type priority() LEGION_IMPURE_RETURN(default_priority);
    };

    template<typename SelfType>
    class RenderStage : public RenderStageBase
    {
    protected:
        /**@brief Create a new entity and return the handle.
         */
        L_NODISCARD ecs::entity_handle createEntity()
        {
            return m_ecs->createEntity();
        }

        template<typename... component_types>
        L_NODISCARD ecs::EntityQuery createQuery()
        {
            return m_ecs->createQuery<component_types...>();
        }

        template<typename event_type, typename... Args, inherits_from<event_type, events::event<event_type>> = 0>
        void raiseEvent(Args... arguments)
        {
            m_eventBus->raiseEvent<event_type>(arguments...);
        }

        void raiseEvent(std::unique_ptr<events::event_base>&& value)
        {
            m_eventBus->raiseEvent(std::move(value));
        }

        void raiseEventUnsafe(std::unique_ptr<events::event_base>&& value, id_type id)
        {
            m_eventBus->raiseEventUnsafe(std::move(value), id);
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD bool checkEvent() const
        {
            return m_eventBus->checkEvent<event_type>();
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD size_type getEventCount() const
        {
            return m_eventBus->getEventCount<event_type>();
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD const event_type& getEvent(index_type index = 0) const
        {
            return m_eventBus->getEvent<event_type>(index);
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        L_NODISCARD const event_type& getLastEvent() const
        {
            return m_eventBus->getLastEvent<event_type>();
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        void clearEvent(index_type index = 0)
        {
            m_eventBus->clearEvent<event_type>(index);
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        void clearLastEvent()
        {
            m_eventBus->clearLastEvent<event_type>();
        }

        template <typename event_type, void(SelfType::* func_type)(event_type*), inherits_from<event_type, events::event<event_type>> = 0>
        void bindToEvent()
        {
            m_eventBus->bindToEvent<event_type>(delegate<void(event_type*)>::template create<SelfType, func_type>(static_cast<SelfType*>(this)));
        }

        template<typename event_type, inherits_from<event_type, events::event<event_type>> = 0>
        void bindToEvent(delegate<void(event_type*)> callback)
        {
            m_eventBus->bindToEvent<event_type>(callback);
        }

    };
}
