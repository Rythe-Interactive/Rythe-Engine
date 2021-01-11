#pragma once
#include <core/core.hpp>
#include <math.h>
namespace legion::rendering
{
    struct lod
    {
        // lod() = default;
        lod(int maxLevel = 4, float maxDistance = 50.0f) : m_maxLOD(maxLevel), m_maxDistance(maxDistance)
        {
            UpdateThresholdLinear();
        }

        int  Level = 0;

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
            float distance = m_maxDistance / (float)m_maxLOD;
            float currentDist = distance;
            m_thresholdLevels = std::vector<float>(m_maxLOD);
            for (size_t i = 0; i < m_maxLOD; i++)
            {
                m_thresholdLevels.at(i) = currentDist;
                currentDist += distance;
            }
            
        }

    private:
        int m_maxLOD;
        float m_maxDistance;
        std::vector<float> m_thresholdLevels;

    };
}
