#include <core/defaults/defaultcomponents.hpp>

namespace legion::core
{
    L_NODISCARD std::tuple<position, rotation, scale> transform::get_local_components()
    {
        OPTICK_EVENT();

        auto& [positionH, rotationH, scaleH] = handles;

        position p = positionH.read();
        rotation r = rotationH.read();
        scale s = scaleH.read();

        auto parent = positionH.entity.get_parent();
        if (!parent.get_id())
            return std::tuple<position, rotation, scale>(p, r, s);

        transform transf = parent.get_component_handles<transform>();
        if (transf)
        {
            position pp = transf.get<position>().read();
            rotation pr = transf.get<rotation>().read();
            scale ps = transf.get<scale>().read();

            return std::tuple<position, rotation, scale>(pp - p, r * math::inverse(pr), s / ps);
        }

        return std::tuple<position, rotation, scale>(p, r, s);
    }

    L_NODISCARD math::mat4 transform::get_local_to_world_matrix()
    {
        OPTICK_EVENT();
        auto& [positionH, rotationH, scaleH] = handles;
        return math::compose(scaleH.read(), rotationH.read(), positionH.read());
    }

}
