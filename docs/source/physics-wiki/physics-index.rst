Physics Getting Started
=======================

The physics subordinate engine (also known as **Diviner**) of Legion-Engine is a powerful way to handle physics events,
collisions and other interesting physics related problems for your game.

To use Diviner make sure to link against ``legion-physics.lib``
If you are on Windows you can do this by right-clicking your project -> Properties -> Linker -> Input 
`More Details <https://stackoverflow.com/a/4446085/5487005>`_

Adding Physics Components
-------------------------


Checking For Collisions
-----------------------

To check for collisions you need to subscribe to one of the physics events using the `eventbus <../architecture-wiki/eventbus.rst>`_
For Example:


.. code-block:: cpp
    :linenos:


    class MySystem : public legion::core::System<MySystem>
    {
    public:
        void setup()
        {
            bindToEvent<legion::physics::trigger_event,&MySystem::onTriggerEvent>();
        }

        void onTriggerEvent(legion::physics::trigger_event* event)
        {
            // ... do things with the event here
        }

    };

You will find the required imports in ``<physics/physics.hpp>`` or explicitly in ``<physics/events/events.hpp>``
