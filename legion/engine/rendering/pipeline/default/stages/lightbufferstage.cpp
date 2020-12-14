#include <rendering/pipeline/default/stages/lightbufferstage.hpp>

namespace legion::rendering
{
   std::unordered_set<ecs::entity_handle> LightBufferStage::m_lightEntities;
   std::vector<detail::light_data> LightBufferStage::m_lights;

    void LightBufferStage::onLightCreate(events::component_creation<light>* event)
    {
        m_lightEntities.insert(event->entity);
    }

    void LightBufferStage::onLightDestroy(events::component_destruction<light>* event)
    {
        m_lightEntities.erase(event->entity);
    }

    void LightBufferStage::setup(app::window& context)
    {
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


        static auto lightsQuery = createQuery<light>();
        lightsQuery.queryEntities();

        for (auto ent : lightsQuery)
            m_lightEntities.insert(ent);
    }

    void LightBufferStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        (void)deltaTime;
        (void)camInput;
        (void)cam;

        static id_type lightsbufferId = nameHash("light buffer");
        static id_type lightCountId = nameHash("light count");
        buffer* lightsBuffer = get_meta<buffer>(lightsbufferId);
        *get_meta<id_type>(lightCountId) = m_lightEntities.size();

        m_lights.resize(m_lightEntities.size());
        int i = 0;
        for (auto ent : m_lightEntities)
        {
            light lght = ent.read_component<light>();
            m_lights[i] = lght.get_light_data(ent.get_component_handle<position>(), ent.get_component_handle<rotation>());
            i++;
        }

        app::context_guard guard(context);
        lightsBuffer->bufferData(m_lights);

    }

    priority_type LightBufferStage::priority()
    {
        return setup_priority;
    }

}
