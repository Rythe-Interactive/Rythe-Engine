#include <audio/systems/audiosystem.hpp>

namespace args::audio
{
    ALCdevice* AudioSystem::data::alDevice = nullptr;
    ALCcontext* AudioSystem::data::alContext = nullptr;
    unsigned int AudioSystem::data::sourceCount = 0;
}
