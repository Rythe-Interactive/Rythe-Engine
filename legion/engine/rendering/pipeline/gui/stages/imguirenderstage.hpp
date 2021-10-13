#pragma once
#include <rendering/pipeline/base/renderstage.hpp>

namespace legion::rendering
{
    class ImGuiStage : public RenderStage<ImGuiStage>
    {
    public:
        void setup(app::window& context) override;
        void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        priority_type priority() override;

        template <class T,void(T::*Func)(app::window&, camera&, const camera::camera_input&, time::span)>
        static void addGuiRender(T* ptr)
        {
            m_onGuiRender.emplace_back<T>(ptr, Func);
        }


    private:

        /** @brief to render imgui things add a function here
         */
        static multicast_delegate<void(app::window&, camera&, const camera::camera_input&, time::span)> m_onGuiRender;
    };
}
