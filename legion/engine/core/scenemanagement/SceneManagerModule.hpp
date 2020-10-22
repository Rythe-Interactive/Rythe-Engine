#pragma once
#include <core/scenemanagement/scenemanager.hpp>

namespace args::core::scenemanagement
{
    class SceneManagerModule: public Module
    {
    public:
        virtual void setup() override
        {
            addProcessChain("SceneModule");
            reportSystem<args::core::scenemanagement::SceneManager>();
        }

        virtual priority_type priority() override
        {
            return 20;
        }
    };
}
