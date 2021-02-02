#pragma once

#include <core/core.hpp>
#include <physics/components/fracturer.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/systems/physics_fracture_test_system.hpp>
#include "../data/explode_event.hpp"
namespace ext
{

    using namespace legion;
    class BigBoom : public System<BigBoom>
    {
        inline static rendering::model_handle m_cubeMatHandle;
        rendering::material_handle m_textureHandle;
        ecs::entity_handle m_affectedEntity;


        enum class BOOM_STRENGTH
        {
            NONE,
            SMALL_BOOM,
            MEDIUM_BOOM,
            BIG_BOOM,
            AN602
        };

        BOOM_STRENGTH m_boom;
    public:
        void setup() override
        {
            m_textureHandle = rendering::MaterialCache::create_material("texture", fs::view("assets://shaders/texture.shs"));
            m_textureHandle.set_param("_texture", rendering::TextureCache::create_texture(fs::view("assets://textures/split-test.png")));
            m_cubeMatHandle = rendering::ModelCache::create_model("cube", fs::view("assets://models/cube.obj"));
            log::debug("{}", ext::evt::explosion_event::id);
            bindToEvent<ext::evt::explosion_event, &BigBoom::onExplosionEvent>();
            createProcess<&BigBoom::onExplodeMesh>("Physics");
        }


        static  ecs::entity_handle createExplosiveEntity(
            physics::cube_collider_params cubeParams,
            rendering::material_handle mat,
            ecs::entity_handle ent
        ) {
            auto entPhyHandle = ent.add_component<physics::physicsComponent>();

            ent.add_component<physics::Fracturer>();

            physics::physicsComponent physicsComponent2;

            physicsComponent2.AddBox(cubeParams);

            entPhyHandle.write(physicsComponent2);

            ent.add_components<rendering::mesh_renderable>(mesh_filter(m_cubeMatHandle.get_mesh()), rendering::mesh_renderer(mat));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));


            auto splitterH = ent.add_component<physics::MeshSplitter>();
            auto splitter = splitterH.read();
            splitter.InitializePolygons(ent);
            splitterH.write(splitter);


            return  ent;
        }
    private:
        void onExplosionEvent(ext::evt::explosion_event* ev)
        {
            static auto query = createQuery<evt::explosion_receiver>();

            ecs::entity_handle handle;
            query.queryEntities();
            for (ecs::entity_handle h : query)
            {
                if (ev->getAffectedName() == h.read_component<evt::explosion_receiver>().name)
                {
                    handle = h;
                }
            }
            if (handle.valid())
            {
                m_affectedEntity = handle;
                createExplosiveEntity(physics::cube_collider_params{ 1,1,1 }, m_textureHandle, m_affectedEntity);

                const std::string strength = ev->getExplosionStrength();
                if (strength == "SMALL")
                {
                    m_boom = BOOM_STRENGTH::SMALL_BOOM;
                }
                else if (strength == "MEDIUM")
                {
                    m_boom = BOOM_STRENGTH::MEDIUM_BOOM;
                }
                else if (strength == "BIG")
                {
                    m_boom = BOOM_STRENGTH::BIG_BOOM;
                }
                else if (strength == "AN602")
                {
                    m_boom = BOOM_STRENGTH::AN602;
                }

            }
        }

        float acc;

        void onExplodeMesh(time::span dt)
        {
            if (m_boom != BOOM_STRENGTH::NONE)
            {
                acc += dt;
                if (acc < 0.05f)
                {
                    return;
                }

            }


            switch (m_boom)
            {
            case BOOM_STRENGTH::NONE: break;
            case BOOM_STRENGTH::SMALL_BOOM:
            {
                log::debug("SMALL BOOM");
                const auto PosH = m_affectedEntity.get_component_handle<position>();
                auto fracturerH = m_affectedEntity.get_component_handle<physics::Fracturer>();
                auto fracturer = fracturerH.read();
                physics::FractureParams param(PosH.read(), 10.0f);
                fracturer.ExplodeEntity(m_affectedEntity, param);
                fracturerH.write(fracturer);
            }
            break;

            case BOOM_STRENGTH::MEDIUM_BOOM:
            {
                log::debug("MEDIUM BOOM");
                const auto PosH = m_affectedEntity.get_component_handle<position>();
                auto fracturerH = m_affectedEntity.get_component_handle<physics::Fracturer>();
                auto fracturer = fracturerH.read();
                physics::FractureParams param(PosH.read(), 50.0f);
                fracturer.ExplodeEntity(m_affectedEntity, param);
                fracturerH.write(fracturer);
            }
            break;
            case BOOM_STRENGTH::BIG_BOOM:
            {
                log::debug("BIG BOOM");
                const auto PosH = m_affectedEntity.get_component_handle<position>();
                auto fracturerH = m_affectedEntity.get_component_handle<physics::Fracturer>();
                auto fracturer = fracturerH.read();
                physics::FractureParams param(PosH.read(), 200.0f);
                fracturer.ExplodeEntity(m_affectedEntity, param);
                fracturerH.write(fracturer);
            }
            break;

            case BOOM_STRENGTH::AN602:
            {
                log::debug("WORLD ENDING BOOM");
                const auto PosH = m_affectedEntity.get_component_handle<position>();
                auto fracturerH = m_affectedEntity.get_component_handle<physics::Fracturer>();
                auto fracturer = fracturerH.read();
                physics::FractureParams param(PosH.read(), 2000.0f);
                fracturer.ExplodeEntity(m_affectedEntity, param);
                fracturerH.write(fracturer);
            }break;
            }
            if (m_boom != BOOM_STRENGTH::NONE)
            {
                acc = 0;
            }

            m_boom = BOOM_STRENGTH::NONE;
        }

    };
}
