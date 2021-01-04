#include <editor-core/editor/editor.hpp>
#include <rendering/rendering.hpp>

/**
 * @file editor.cpp
 */

namespace legion::editor
{
    Engine* Editor::m_engine;

    Editor::Editor(Engine* engine)
    {
        m_engine = engine;
    }

    Engine* Editor::getEngine()
    {
        return m_engine;
    }
}
