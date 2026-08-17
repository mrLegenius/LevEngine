#pragma once

namespace LevEngine::Editor
{
    //<--- One reversible editor action. Undo puts back what was there before it, Redo does it again ---<<
    class UndoCommand
    {
    public:
        virtual ~UndoCommand() = default;

        virtual void Undo() = 0;
        virtual void Redo() = 0;

        //<--- Shown in the Edit menu, so it reads like the action it takes back ---<<
        [[nodiscard]] virtual String GetName() const = 0;
    };
}
