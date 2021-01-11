#pragma once
#include <core/core.hpp>
#include<rendering/components/lod.hpp>
#include<rendering/components/camera.hpp>
#include<math.h>
namespace legion::rendering
{


    class LODManager : public System<LODManager>
    {

        void setup()
        {
            createProcess<&LODManager::update>("Update");
        }
        void update(time::span deltaTime)
        {
            UpdateCam();
            m_query.queryEntities();

            for (ecs::entity_handle entity : m_query)
            {
                float distance = CalculateDistance(entity.get_component_handle<position>());
                auto currentLOD = entity.get_component_handle<lod>().read();
                currentLOD.UpdateLOD(distance);
                log::debug(currentLOD.Level);
                entity.get_component_handle<lod>().write(currentLOD);
            }


            for (ecs::entity_handle entity : m_query)
            {
                log::debug(entity.get_component_handle<lod>().read().Level);
            }

        }

    private:
        void UpdateCam()
        {
            m_CamQuery.queryEntities();
            for (ecs::entity_handle entity : m_CamQuery)
            {
                if (entity.has_component<position>())
                {
                    //   log::debug("found camera position");
                       //  entity.get_component_handles<transform>().get
                    m_camPosition = entity.get_component_handle<position>().read();
                }

            }

        }
        float CalculateDistance(ecs::component_handle<position> pos)
        {
            return math::distance(pos.read(), m_camPosition);
        }
        math::vec3 m_camPosition;
        ecs::EntityQuery m_query = createQuery<transform, lod>();
        ecs::EntityQuery m_CamQuery = createQuery<camera>();

    };
}
