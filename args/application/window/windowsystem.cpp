#include <application/window/windowsystem.hpp>

namespace args::application
{
    sparse_map<GLFWwindow*, ecs::component_handle<window>> WindowSystem::data::m_windowComponents;
    sparse_map<GLFWwindow*, events::EventBus*> WindowSystem::data::m_windowEventBus;
    async::readonly_rw_spinlock WindowSystem::data::m_creationLock;
}
