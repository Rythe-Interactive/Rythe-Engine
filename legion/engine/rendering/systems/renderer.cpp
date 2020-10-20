#include <rendering/systems/renderer.hpp>

namespace legion::rendering
{
    async::readonly_rw_spinlock Renderer::debugLinesLock;
    thread_local std::unordered_set<debug::debug_line>* Renderer::localLines;
    std::unordered_map<std::thread::id, std::unordered_set<debug::debug_line>*> Renderer::debugLines;
}
