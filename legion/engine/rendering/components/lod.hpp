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
        //    UpdateThresholdLinear();
        }
        int MaxLod;
        int Level = 0;
        int MaxTreeLevel = 0;

        bool isInitialized = false;
        float m_maxDistance;
        std::vector<float> m_thresholdLevels;
    };
}
