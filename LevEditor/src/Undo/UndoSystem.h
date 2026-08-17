#pragma once

namespace LevEngine
{
    class Scene;
}

namespace LevEngine::Editor
{
    class UndoCommand;

    //<--- The editor's history. Everything that changes a scene from the editor is pushed here,
    //otherwise there is nothing to go back to ---<<
    class UndoSystem
    {
    public:
        //<--- Commands address entities by UUID, and those UUIDs mean nothing in the next scene, so
        //the history is dropped whenever the active scene changes. Called once a frame, because the
        //event that announces a load lives behind symbols the engine does not export ---<<
        static void SyncWithActiveScene();
        static void Shutdown();

        static void Push(const Ref<UndoCommand>& command);

        static void Undo();
        static void Redo();

        [[nodiscard]] static bool CanUndo() { return !s_UndoStack.empty(); }
        [[nodiscard]] static bool CanRedo() { return !s_RedoStack.empty(); }

        [[nodiscard]] static String GetUndoName();
        [[nodiscard]] static String GetRedoName();

        static void Clear();

        //<--- Off outside of edit mode: play mode reloads the scene when it ends, so there is
        //nothing there worth going back to ---<<
        static void SetRecordingEnabled(bool enabled) { s_IsEnabled = enabled; }

        //<--- False while a command is being undone or redone, so a restore does not record itself ---<<
        [[nodiscard]] static bool IsRecording();

        //<--- Bumped on every change to the stacks. Whoever holds a "before" snapshot watches this to
        //know that someone else has already recorded and its own snapshot is stale ---<<
        [[nodiscard]] static uint64_t GetRevision() { return s_Revision; }

    private:
        //<--- Snapshots are cheap to hold but not free, and nobody walks back a hundred edits ---<<
        static constexpr size_t k_MaxDepth = 128;

        static inline Vector<Ref<UndoCommand>> s_UndoStack;
        static inline Vector<Ref<UndoCommand>> s_RedoStack;

        //<--- Compared, never dereferenced ---<<
        static inline const Scene* s_Scene = nullptr;

        static inline bool s_IsReplaying = false;
        static inline bool s_IsEnabled = true;
        static inline uint64_t s_Revision = 0;
    };
}
