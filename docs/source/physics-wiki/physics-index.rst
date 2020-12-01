Physics Getting Started
=======================

The physics subordinate engine (also known as **Diviner**) of Legion-Engine is a powerful way to handle physics events,
collisions and other interesting physics related problems for your game.

To use Diviner make sure to link against ``legion-physics.lib``
If you are on Windows you can do this by right-clicking your project -> Properties -> Linker -> Input 
`More Details <https://stackoverflow.com/a/4446085/5487005>`_

Adding Physics Components
-------------------------
To achieve anything with the physics-engine we need to attach physics components to our entities.
While it is not particularly complicated it is a bit more involved than adding regular components.
Take a look:



.. code-block:: cpp
    :linenos:


    //create an entity 
    auto ent = m_ecs->createEntity();

    //attach a physics component to the entity
    auto entPhyHande = ent.add_component<physics::physicsComponent>();

    //init the physics Component
    physics::physicsComponent physicsComponent;
    physics::physicsComponent::init(physicsComponent);

    // Add a Box and write back to the Entity
    physicsComponent.AddBox(cubeParams);
    entPhyHande.write(physicsComponent); 


As you can see we need to call ``physics::physicsComponent::init`` on the component, which assures that the physics component is properly setup.
After that you can work with it as you would with any other component. here we just simply add a box to the physicsComponent and then write it
to the handle.



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
