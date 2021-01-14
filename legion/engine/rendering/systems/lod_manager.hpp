#pragma once
#include <core/core.hpp>
#include<rendering/components/lod.hpp>
#include<rendering/components/camera.hpp>
#include<math.h>
namespace legion::rendering
{
    /**@class LODManager
     * @brief system that updates the LOD component
     */
    class LODManager : public System<LODManager>
    {
        void setup()
        {
            createProcess<&LODManager::update>("Update");
        }
        /** @brief Update queries all entities with LOD components, caclulates their distance and updates the LOD
          */
        void update(time::span deltaTime)
        {
            //update camera position first
            UpdateCam();

            //iterate all entities with an LOD
            m_query.queryEntities();
            for (ecs::entity_handle entity : m_query)
            {
                //get distance for current entity
                float distance = CalculateDistance(entity.get_component_handle<position>());
                //read the LOD
                auto currentLOD = entity.get_component_handle<lod>().read();
                //make sure it is initialized
                if (!currentLOD.isInitialized) UpdateThresholdLinear(currentLOD);
                //update LOD with calculated distance
                UpdateLOD(currentLOD, distance);
                //write any LOD changes
                entity.get_component_handle<lod>().write(currentLOD);
            }
        }

    private:
        //checks if current distance should display a different LOD than the current LOD
        void UpdateLOD(lod& lodComponent, float distance)
        {
            int currentLevel = 0;
            for (float threshold : lodComponent.m_thresholdLevels)
            {
                if (threshold > distance)
                {
                    lodComponent.Level = currentLevel;
                    return;
                }
                currentLevel++;
            }
        }
        //based on the max lod level and the max lod distance calculate the stepping points to update the LOD
        void UpdateThresholdLinear(lod& lodComponent)
        {
            float distance = lodComponent.m_maxDistance / (float)lodComponent.MaxLod;
            float currentDist = distance;
            lodComponent.m_thresholdLevels = std::vector<float>(lodComponent.MaxLod);
            for (size_t i = 0; i < lodComponent.MaxLod; i++)
            {
                lodComponent.m_thresholdLevels.at(i) = currentDist;
                currentDist += distance;
            }
            lodComponent.isInitialized = true;
        }

        //updates camera posiiton
        void UpdateCam()
        {
            m_CamQuery.queryEntities();
            for (ecs::entity_handle entity : m_CamQuery)
            {
                if (entity.has_component<position>())
                {
                    m_camPosition = entity.get_component_handle<position>().read();
                }
            }
        }
        //calculates distince between cam and input positon
        float CalculateDistance(ecs::component_handle<position> pos)
        {
            return math::distance(pos.read(), m_camPosition);
        }
        math::vec3 m_camPosition;
        //query for the lod components
        ecs::EntityQuery m_query = createQuery<transform, lod>();
        //query for the cam
        ecs::EntityQuery m_CamQuery = createQuery<camera>();

    };
}
