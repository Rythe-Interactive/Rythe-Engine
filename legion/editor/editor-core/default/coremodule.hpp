#pragma once
#include <editor-core/editor/editormodule.hpp>

namespace legion::editor
{
    class CoreEditorModule : public EditorModule<CoreModule>
    {
    public:
        virtual void setup() override;
    };

}
