#include <core/platform/platform.hpp>
#include <core/core.hpp>

namespace args::scenemanagement
{
    struct Scene
    {
        std::unordered_map<int, std::string> sceneObjects;
        int objectCount = 0;

        template<typename Archive>
        void serialize(Archive& archive)
        {
            
         
            //archive(cereal::make_nvp("SceneRoot",sceneObjects));
        }
    };
}
