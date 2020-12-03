#include <rendering/systems/oldrenderer.hpp>

namespace legion::rendering
{
    async::readonly_rw_spinlock OldRenderer::debugLinesLock;
    thread_local std::unordered_set<debug::debug_line>* OldRenderer::localLines;
    std::unordered_map<std::thread::id, std::unordered_set<debug::debug_line>*> OldRenderer::debugLines;
}
