#pragma once

namespace LevEngine::Editor
{
    //<--- Turns a stream of per frame changes into undo steps. Whoever draws a widget that writes
    //straight into a component tells the tracker when the user is holding it: one grab of a slider,
    //or one drag of a gizmo, is one step, and what happens in between is not worth a step of its own.
    //
    //While the user is only pointing at the panel the snapshot is kept fresh, so what the editor
    //itself changes -- a light that follows the sky, a value a system writes -- never turns into
    //something to undo. Away from the panel nothing is captured at all, which is what keeps a heavy
    //entity from being serialized every frame of its life ---<<
    class EntityEditTracker
    {
    public:
        //<--- The verb the undo step is named after, "Edit Player" and so on ---<<
        explicit EntityEditTracker(String actionName) : m_ActionName(Move(actionName)) { }

        //<--- isInteracting: a widget is being held right now. isEngaged: the user is pointing at
        //the thing that edits this entity, so an interaction could start at any frame ---<<
        void Update(Entity entity, bool isInteracting, bool isEngaged);
        void Reset();

    private:
        String m_ActionName;
        String m_Baseline;
        UUID m_Entity{ 0 };
        uint64_t m_Revision = 0;
        bool m_HasInteracted = false;
    };
}
