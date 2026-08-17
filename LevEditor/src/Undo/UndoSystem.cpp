#include "pch.h"
#include "UndoSystem.h"

#include "UndoCommand.h"

namespace LevEngine::Editor
{
    void UndoSystem::SyncWithActiveScene()
    {
        const Scene* scene = SceneManager::GetActiveScene().get();

        if (scene == s_Scene) return;

        s_Scene = scene;
        Clear();
    }

    void UndoSystem::Shutdown()
    {
        s_Scene = nullptr;
        Clear();
    }

    bool UndoSystem::IsRecording()
    {
        return s_IsEnabled && !s_IsReplaying;
    }

    void UndoSystem::Push(const Ref<UndoCommand>& command)
    {
        if (!command || !IsRecording()) return;

        //<--- A new action is a new branch of history, whatever was undone is not coming back ---<<
        s_RedoStack.clear();

        s_UndoStack.push_back(command);

        if (s_UndoStack.size() > k_MaxDepth)
            s_UndoStack.erase(s_UndoStack.begin());

        s_Revision++;
    }

    void UndoSystem::Undo()
    {
        if (s_UndoStack.empty() || !IsRecording()) return;

        const auto command = s_UndoStack.back();
        s_UndoStack.pop_back();

        s_IsReplaying = true;
        command->Undo();
        s_IsReplaying = false;

        s_RedoStack.push_back(command);
        s_Revision++;

        Log::CoreTrace("Undo '{0}'", command->GetName());
    }

    void UndoSystem::Redo()
    {
        if (s_RedoStack.empty() || !IsRecording()) return;

        const auto command = s_RedoStack.back();
        s_RedoStack.pop_back();

        s_IsReplaying = true;
        command->Redo();
        s_IsReplaying = false;

        s_UndoStack.push_back(command);
        s_Revision++;

        Log::CoreTrace("Redo '{0}'", command->GetName());
    }

    String UndoSystem::GetUndoName()
    {
        return s_UndoStack.empty() ? String{} : s_UndoStack.back()->GetName();
    }

    String UndoSystem::GetRedoName()
    {
        return s_RedoStack.empty() ? String{} : s_RedoStack.back()->GetName();
    }

    void UndoSystem::Clear()
    {
        s_UndoStack.clear();
        s_RedoStack.clear();
        s_Revision++;
    }
}
