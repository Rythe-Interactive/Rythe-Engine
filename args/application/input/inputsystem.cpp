#include <application/input/inputsystem.hpp>

namespace args::application
{
    math::dvec2 InputSystem::data::m_mousePos;
    math::dvec2 InputSystem::data::m_mouseDelta;

    std::set<int> InputSystem::data::m_presentGamepads;
    sparse_map<inputmap::method, sparse_map<id_type,InputSystem::action_data>> InputSystem::data::m_actions;

    sparse_map<inputmap::method, sparse_map<id_type,InputSystem::axis_data>> InputSystem::data::m_axes;
}
