#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>

using namespace legion;



class PointCloudTestSystem final : public legion::core::System<PointCloudTestSystem>
{

    virtual void setup() override
    {
        //        auto result = fs::view("assets://models/Cube.obj").get();
             /*   auto result = fs::view("assets://models/Cube.obj").load_as<>

                if (result == common::valid)
                {
                    auto resource = result.decay();

                }*/

        log::debug("Point Cloud Test System setup");
        /*    app::window window = m_ecs->world.get_component_handle<app::window>().read();
            window.enableCursor(false);
            window.show();*/


    }



};





