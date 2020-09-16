#pragma once
#include <core/scene management/scene.hpp>

namespace args::scenemanagement
{
    class ARGS_API SceneManager
    {
    public:
        int sceneCount;
        std::unordered_map<std::string, args::scenemanagement::Scene> sceneList;
    };
}
