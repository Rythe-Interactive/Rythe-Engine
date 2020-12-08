#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>

#include <Voro++/voro++.hh>
#include <Voro++/common.hh>


#include <rendering/systems/pointcloud_particlesystem.hpp>
#include <rendering/components/particle_emitter.hpp>


using namespace legion;

class wall_torus : public voro::wall {
public:

    // The wall constructor initializes constants for the major and
    // minor axes of the torus. It also initializes the wall ID
    // number that is used when the plane cuts are made. This is
    // only tracked with the voronoicell_neighbor class and is
    // ignored otherwise. It can be omitted, and then an arbitrary
    // value of -99 is used.
    wall_torus(double imjr, double imnr, int iw_id = -99)
        : w_id(iw_id), mjr(imjr), mnr(imnr) {};

    // This returns true if a given vector is inside the torus, and
    // false if it is outside. For the current example, this
    // routine is not needed, but in general it would be, for use
    // with the point_inside() routine in the container class.
    bool point_inside(double x, double y, double z) {
        double temp = sqrt(x * x + y * y) - mjr;
        return temp * temp + z * z < mnr* mnr;
    }

    // This template takes a reference to a voronoicell or
    // voronoicell_neighbor object for a particle at a vector
    // (x,y,z), and makes a plane cut to to the object to account
    // for the toroidal wall
    template<class vc_class>
    inline bool cut_cell_base(vc_class& c, double x, double y, double z) {
        double orad = sqrt(x * x + y * y);
        double odis = orad - mjr;
        double ot = odis * odis + z * z;

        // Unless the particle is within 1% of the major
        // radius, then a plane cut is made
        if (ot > 0.01 * mnr) {
            ot = 2 * mnr / sqrt(ot) - 2;
            z *= ot;
            odis *= ot / orad;
            x *= odis;
            y *= odis;
            return c.nplane(x, y, z, w_id);
        }
        return true;
    }

    // These virtual functions are called during the cell
    // computation in the container class. They call instances of
    // the template given above.
    bool cut_cell(voro::voronoicell& c, double x,
        double y, double z) {
        return cut_cell_base(c, x, y, z);
    }
    bool cut_cell(voro::voronoicell_neighbor& c, double x,
        double y, double z) {
        return cut_cell_base(c, x, y, z);
    }
private:
    // The ID number associated with the wall
    const int w_id;
    // The major radius of the torus
    const double mjr;
    // The minor radius of the torus
    const double mnr;
};

double rnd() { return double(rand()) / RAND_MAX; }

class TestSystem2 final : public System<TestSystem2>
{
public:
    TestSystem2()
    {
        log::filter(log::severity::debug);
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1);
    }

    std::vector<std::vector<math::vec4>> voronoi;

    virtual void setup()
    {
        rendering::model_handle cube;
        rendering::material_handle flatGreen;
        rendering::material_handle vertexColor;
        rendering::material_handle directionalLightMH;


        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        /*  {
              async::readwrite_guard guard(*window.lock);
              app::ContextHelper::makeContextCurrent(window);


              auto colorshader = rendering::ShaderCache::create_shader("color", "assets://shaders/color.shs"_view);
              directionalLightMH = rendering::MaterialCache::create_material("directional light", colorshader);
              directionalLightMH.set_param("color", math::color(1, 1, 0.8f));

              cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
              vertexColor = rendering::MaterialCache::create_material("vertex color", "assets://shaders/vertexcolor.shs"_view);


              std::vector<math::vec3> positions{
                  math::vec3(0,1.0f,0),
                  math::vec3(0,1.25f,0),
                  math::vec3(0,1.5f,0),
                  math::vec3(0,1.75f,0),

                  math::vec3(1,1.0f,0),
                  math::vec3(1,1.25f,0),
                  math::vec3(1,1.5f,0),
                  math::vec3(1,1.75f,0),

                  math::vec3(1,1.0f,1),
                  math::vec3(1,1.25f,1),
                  math::vec3(1,1.5f,1),
                  math::vec3(1,1.75f,1),

                  math::vec3(0,1.0f,1),
                  math::vec3(0,1.25f,1),
                  math::vec3(0,1.5f,1),
                  math::vec3(0,1.75f,1)
              };
              pointCloudParameters params{
              params.startingSize = math::vec3(0.2f),
              params.particleMaterial = vertexColor,
                  params.particleModel = cube
              };
              auto pointcloud = rendering::ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>("point_cloud", params, positions);

  #pragma region entities

              {
                  auto ent = createEntity();
                  ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale(1));
                  rendering::particle_emitter emitter = ent.add_component<rendering::particle_emitter>().read();
                  emitter.particleSystemHandle = pointcloud;
                  ent.get_component_handle<rendering::particle_emitter>().write(emitter);
              }

  #pragma endregion

          }*/
        std::vector<math::vec3> points;
        double x, y, z;
        double width = 10, height = 10, depth = 10;
        for (int i = 0; i < 40; i++)
        {
            x = -width + rnd() * (width - (-width));
            y = -height + rnd() * (height - (-height));
            z = -depth + rnd() * (depth - (-depth));
            points.push_back(math::vec3(x, y, z));
        }
        voronoi = physics::PhysicsStatics::GenerateVoronoi(points);

        createProcess<&TestSystem2::update>("Update");
    }

    void update(time::span deltaTime)
    {
        for (auto point : voronoi)
        {
            debug::drawLine(point[0], point[1], math::colors::magenta);
        }
    }
};
