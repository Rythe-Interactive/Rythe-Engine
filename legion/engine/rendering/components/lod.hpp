#pragma once
#include <core/core.hpp>
#include <math.h>
namespace legion::rendering
{
    struct lod
    {
        // lod() = default;
        lod(int maxLevel = 5, float maxDistance = 50.0f) : MaxLod(maxLevel), m_maxDistance(maxDistance)
        {
            UpdateThresholdLinear();
        }
        int MaxLod;
        int Level = 0;
        int MaxTreeLevel = 0;



        void UpdateLOD(float distance)
        {
            int currentLevel = 0;
            for (float threshold : m_thresholdLevels)
            {
                if (threshold > distance)
                {
                    Level = currentLevel;
                    return;
                }
                currentLevel++;
            }
        }
        void UpdateThresholdLinear()
        {
            float distance = m_maxDistance / (float)MaxLod;
            float currentDist = distance;
            m_thresholdLevels = std::vector<float>(MaxLod);
            for (size_t i = 0; i < MaxLod; i++)
            {
                m_thresholdLevels.at(i) = currentDist;
                currentDist += distance;
            }
            
        }

    private:
        float m_maxDistance;
        std::vector<float> m_thresholdLevels;

    };
}
