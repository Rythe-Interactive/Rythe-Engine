#pragma once
#include <application/application.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/pipeline/base/pipelinebase.hpp>

#define setup_priority 64
#define opaque_priority 32
//      default_priority 0
#define transparent_priority -16
#define volumetric_priority -32
#define post_fx_priority -48
#define ui_priority -64
#define submit_priority -96

namespace legion::rendering
{
    class RenderPipelineBase;

    class RenderStageBase
    {
        friend class Renderer;
    private:
        bool m_isInitialized = false;

    protected:
        static ecs::EcsRegistry* m_ecs;
        static schd::Scheduler* m_scheduler;
        static events::EventBus* m_eventBus;

        virtual void setup(app::window& context) LEGION_PURE;

    public:
        static RenderPipelineBase* m_pipeline;

        inline bool isInitialized() { return m_isInitialized; }

        inline void init(app::window& context)
        {
            OPTICK_EVENT("Setup render stage");
            m_isInitialized = true;
            setup(context);
        }

        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) LEGION_PURE;
        virtual priority_type priority() LEGION_IMPURE_RETURN(default_priority);

    protected:
        void abort();

        template<typename T>
        L_NODISCARD bool has_meta(const std::string& name);

        template<typename T, typename... Args>
        T* create_meta(const std::string& name, Args&&... args);

        template<typename T>
        L_NODISCARD T* get_meta(const std::string& name);

        template<typename T>
        L_NODISCARD bool has_meta(id_type nameHash);

        template<typename T, typename... Args>
        T* create_meta(id_type nameHash, Args&&... args);

        template<typename T>
        L_NODISCARD T* get_meta(id_type nameHash);

        framebuffer* addFramebuffer(const std::string& name, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD bool hasFramebuffer(const std::string& name, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD framebuffer* getFramebuffer(const std::string& name);
        framebuffer* addFramebuffer(id_type nameHash, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD bool hasFramebuffer(id_type nameHash, GLenum target = GL_FRAMEBUFFER);
        L_NODISCARD framebuffer* getFramebuffer(id_type nameHash);
    };

    template<typename SelfType>
    class RenderStage : public RenderStageBase
    {
    protected:
        /**@brief Create a new entity and return the handle.
         */
        L_NODISCARD ecs::entity_handle createEntity()
        {
            OPTICK_EVENT();
            return m_ecs->createEntity();
        }

        template<typename... component_types>
        L_NODISCARD ecs::EntityQuery createQuery()
        {
            OPTICK_EVENT();
            return m_ecs->createQuery<component_types...>();
        }

        L_NODISCARD ecs::EntityQuery createQuery(const hashed_sparse_set<id_type>& componentTypes)
        {
            OPTICK_EVENT();
            return m_ecs->createQuery(componentTypes);
        }

        template<typename event_type, typename... Args CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        void raiseEvent(Args... arguments)
        {
            OPTICK_EVENT();
            m_eventBus->raiseEvent<event_type>(arguments...);
        }

        void raiseEvent(std::unique_ptr<events::event_base>&& value)
        {
            OPTICK_EVENT();
            m_eventBus->raiseEvent(std::move(value));
        }

        void raiseEventUnsafe(std::unique_ptr<events::event_base>&& value, id_type id)
        {
            OPTICK_EVENT();
            m_eventBus->raiseEventUnsafe(std::move(value), id);
        }

        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        L_NODISCARD bool checkEvent() const
        {
            OPTICK_EVENT();
            return m_eventBus->checkEvent<event_type>();
        }

        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        L_NODISCARD size_type getEventCount() const
        {
            OPTICK_EVENT();
            return m_eventBus->getEventCount<event_type>();
        }

        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        L_NODISCARD const event_type& getEvent(index_type index = 0) const
        {
            OPTICK_EVENT();
            return m_eventBus->getEvent<event_type>(index);
        }
        
        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        L_NODISCARD const event_type& getLastEvent() const
        {
            OPTICK_EVENT();
            return m_eventBus->getLastEvent<event_type>();
        }

        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        void clearEvent(index_type index = 0)
        {
            OPTICK_EVENT();
            m_eventBus->clearEvent<event_type>(index);
        }

        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        void clearLastEvent()
        {
            OPTICK_EVENT();
            m_eventBus->clearLastEvent<event_type>();
        }

        template<typename event_type, void(SelfType::* func_type)(event_type*) CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        void bindToEvent()
        {
            OPTICK_EVENT();
            m_eventBus->bindToEvent<event_type>(delegate<void(event_type*)>::template create<SelfType, func_type>(static_cast<SelfType*>(this)));
        }

        template<typename event_type CNDOXY(inherits_from<event_type, events::event<event_type>> = 0)>
        void bindToEvent(delegate<void(event_type*)> callback)
        {
            OPTICK_EVENT();
            m_eventBus->bindToEvent<event_type>(callback);
        }

    };
}

#include <rendering/pipeline/base/renderstage.inl>
