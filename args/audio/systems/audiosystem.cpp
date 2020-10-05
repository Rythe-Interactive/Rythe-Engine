#include <audio/systems/audiosystem.hpp>

namespace args::audio
{
    ALCdevice* AudioSystem::alDevice = nullptr;
    ALCcontext* AudioSystem::alContext = nullptr;
    unsigned int AudioSystem::sourceCount = 0;
    unsigned int AudioSystem::listenerCount = 0;
}
