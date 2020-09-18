#pragma once
#include <core/scene management/scenemanager.hpp>

namespace args::scenemanagement
{
    class SceneManagerModule: public Module
    {
    public:
        SceneManager sceneManager;
        virtual void setup() override
        {
            addProcessChain("SceneModule");
            reportSystem<SceneManager>();
        }

        virtual priority_type priority() override
        {
            return 20;
        }
    };
}
