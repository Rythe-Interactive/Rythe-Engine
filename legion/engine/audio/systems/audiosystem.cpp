#include <audio/systems/audiosystem.hpp>

namespace legion::audio
{
    async::spinlock AudioSystem::contextLock;
    ALCdevice* AudioSystem::alDevice = nullptr;
    ALCcontext* AudioSystem::alcContext = nullptr;
    unsigned int AudioSystem::sourceCount = 0;
    unsigned int AudioSystem::listenerCount = 0;
}
