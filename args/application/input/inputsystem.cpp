#include <application/input/inputsystem.hpp>

namespace args::application
{
    math::dvec2 InputSystem::data::m_mousePos;
    math::dvec2 InputSystem::data::m_mouseDelta;

    std::set<int> InputSystem::data::m_presentGamepads;
    sparse_map<inputmap::method, sparse_map<id_type,
        std::tuple<delegate<void(InputSystem*, bool, inputmap::modifier_keys, inputmap::method, float)>, float>>
        > InputSystem::data::m_actions;

    sparse_map<inputmap::method, sparse_map<id_type,
        std::tuple<delegate<void(InputSystem*, float, inputmap::modifier_keys, inputmap::method)>, float, inputmap::modifier_keys, inputmap::method>>
        > InputSystem::data::m_axes;
}
