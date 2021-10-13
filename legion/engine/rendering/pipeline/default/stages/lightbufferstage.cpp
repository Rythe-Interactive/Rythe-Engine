#include <rendering/pipeline/default/stages/lightbufferstage.hpp>

namespace legion::rendering
{
   async::spinlock LightBufferStage::m_lightEntitiesLock;
   std::unordered_set<ecs::entity> LightBufferStage::m_lightEntities;
   std::vector<detail::light_data> LightBufferStage::m_lights;

    void LightBufferStage::onLightCreate(events::component_creation<light>& event)
    {
        OPTICK_EVENT();
        std::lock_guard guard(m_lightEntitiesLock);
        m_lightEntities.insert(event.entity);
    }

    void LightBufferStage::onLightDestroy(events::component_destruction<light>& event)
    {
        OPTICK_EVENT();
        std::lock_guard guard(m_lightEntitiesLock);
        m_lightEntities.erase(event.entity);
    }

    void LightBufferStage::setup(app::window& context)
    {
        OPTICK_EVENT();
        buffer lightsBuffer;

        {
            app::context_guard guard(context);
            lightsBuffer = buffer(GL_SHADER_STORAGE_BUFFER, sizeof(detail::light_data) * 128, nullptr, GL_DYNAMIC_DRAW);
            lightsBuffer.bindBufferBase(SV_LIGHTS);
        }

        create_meta<buffer>("light buffer", lightsBuffer);
        create_meta<size_type>("light count");

        bindToEvent<events::component_creation<light>, &LightBufferStage::onLightCreate>();
        bindToEvent<events::component_destruction<light>, &LightBufferStage::onLightDestroy>();

        static ecs::filter<light> lightsQuery{};

        std::lock_guard guard(m_lightEntitiesLock);
        for (auto ent : lightsQuery)
            m_lightEntities.insert(ent);
    }

    void LightBufferStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        OPTICK_EVENT();
        (void)deltaTime;
        (void)camInput;
        (void)cam;

        static id_type lightsbufferId = nameHash("light buffer");
        static id_type lightCountId = nameHash("light count");
        buffer* lightsBuffer = get_meta<buffer>(lightsbufferId);

        {
            std::lock_guard guard(m_lightEntitiesLock);
            *get_meta<id_type>(lightCountId) = m_lightEntities.size();

            m_lights.resize(m_lightEntities.size());
            int i = 0;
            for (auto ent : m_lightEntities)
            {
                light lght = ent.get_component<light>();
                m_lights[i] = lght.get_light_data(ent.get_component<position>(), ent.get_component<rotation>());
                i++;
            }
        }

        app::context_guard guard(context);
        lightsBuffer->bufferData(m_lights);

    }

    priority_type LightBufferStage::priority()
    {
        return setup_priority;
    }

}
