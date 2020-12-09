#include <rendering/pipeline/default/stages/lightbufferstage.hpp>

namespace legion::rendering
{
    void LightBufferStage::onLightCreate(events::component_creation<light>* event)
    {
        light lght = event->entity.read_component<light>();
        lights.insert(event->entity, lght.get_light_data(event->entity.get_component_handle<position>(), event->entity.get_component_handle<rotation>()));
    }

    void LightBufferStage::onLightDestroy(events::component_destruction<light>* event)
    {
        lights.erase(event->entity);
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
    }

    void LightBufferStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        (void)deltaTime;
        (void)camInput;
        (void)cam;

        static id_type lightsbufferId = nameHash("light buffer");
        static id_type lightCountId = nameHash("light count");
        buffer* lightsBuffer = get_meta<buffer>(lightsbufferId);
        *get_meta<id_type>(lightCountId) = lights.size();

        app::context_guard guard(context);
        lightsBuffer->bufferData(lights.values());

    }

    priority_type LightBufferStage::priority()
    {
        return PRIORITY_MAX;
    }

}
