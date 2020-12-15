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

        template <class T,void(T::*Func)()>
        static void addGuiRender(T* ptr)
        {
            m_onGuiRender += delegate<void()>::create<T,Func>(ptr);
        }


    private:

        /** @brief to render imgui things add a function here
         */
        static multicast_delegate<void()> m_onGuiRender;
    };
}
