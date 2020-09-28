#pragma once
#include <audio/detail/engine_include.hpp>
#include <AL/al.h>
#include <AL/alc.h>

namespace args::audio
{
    class AudioSystem final : System<AudioSystem>
    {
    public:
        virtual void setup()
        {
            createProcess<&AudioSystem::update>("Audio");
            bindToEvent<events::component_creation<position>, &AudioSystem::onComponentCreate>();
        }

        void onComponentCreate(events::component_creation<position>* event)
        {
            auto handle = event->entity.get_component_handle<position>();
            position p = handle.read();
            // do something with p.
            handle.write(p);
        }

        void update(time::span deltatime);

    private:
        static ALCcontext* m_alContext;
    };
}

#include <audio/systems/audiosystem.inl>
