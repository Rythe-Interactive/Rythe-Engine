#pragma once
#include <core/scenemanagement/scenemanager.hpp>

namespace legion::core::scenemanagement
{
    class SceneManagerModule: public Module
    {
    public:
        virtual void setup() override
        {
            addProcessChain("SceneModule");
            reportSystem<legion::core::scenemanagement::SceneManager>();
        }

        virtual priority_type priority() override
        {
            return 20;
        }
    };
}
