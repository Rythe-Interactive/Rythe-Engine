#define LEGION_ENTRY
#define LEGION_KEEP_CONSOLE
#define LEGION_LOW_POWER
#define LEGION_MIN_THREADS 6 // Update, Rendering, Input, Audio, Physics, TestChain

#include <editor-core/core.hpp>


using namespace legion;
using namespace legion::editor;

void LEGION_CCONV reportEditorModules(Editor* editor)
{
    editor->reportModule<CoreEditorModule>();
}
