#pragma once
#include <core/scene management/scene.hpp>

namespace args::scenemanagement
{
    class ARGS_API SceneManager
    {
    public:
        int sceneCount;
        std::unordered_map<std::string, args::scenemanagement::Scene> sceneList;
        std::unique_ptr<args::scenemanagement::Scene> scene = std::unique_ptr<args::scenemanagement::Scene>();

        SceneManager()
        {
            //scene = std::unique_ptr<args::scenemanagement::Scene>();
        }

        std::unique_ptr<args::scenemanagement::Scene> createScene()
        {
            std::unique_ptr<args::scenemanagement::Scene> _scene = std::unique_ptr<args::scenemanagement::Scene>();
            return _scene;
        }

        void loadScene()
        {

        }

        void saveScene()
        {

        }
    };
}
