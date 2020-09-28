#include <audio/systems/audiosystem.hpp>

namespace args::audio
{
    ALCdevice* AudioSystem::data::m_alDevice = nullptr;
    ALCcontext* AudioSystem::data::m_alContext = nullptr;
    unsigned int AudioSystem::data::m_sourceCount = 0;
}
