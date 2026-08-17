#pragma once
#include "UndoCommand.h"

namespace LevEngine::Editor
{
    //<--- A change to what one entity holds: its values, its components, its place in the hierarchy.
    //Both sides are snapshots, so the same command walks either way ---<<
    class EntityStateCommand final : public UndoCommand
    {
    public:
        EntityStateCommand(const UUID entity, String before, String after, String name)
            : m_Entity(entity), m_Before(Move(before)), m_After(Move(after)), m_Name(Move(name)) { }

        void Undo() override { Apply(m_Before); }
        void Redo() override { Apply(m_After); }

        [[nodiscard]] String GetName() const override { return m_Name; }

    private:
        void Apply(const String& state) const;

        UUID m_Entity;
        String m_Before;
        String m_After;
        String m_Name;
    };

    //<--- An entity the editor put in the scene. Undoing takes the branch away, redoing brings it
    //back with the UUIDs it had ---<<
    class EntityCreatedCommand final : public UndoCommand
    {
    public:
        //<--- Built after the entity is there, everything it holds is part of the action ---<<
        EntityCreatedCommand(Entity entity, String name);

        void Undo() override;
        void Redo() override;

        [[nodiscard]] String GetName() const override { return m_Name; }

    private:
        UUID m_Entity;
        String m_Snapshot;
        String m_Name;
    };

    //<--- An entity the editor took out of the scene. Built before it goes, that is the only moment
    //there is anything left to capture ---<<
    class EntityDestroyedCommand final : public UndoCommand
    {
    public:
        EntityDestroyedCommand(Entity entity, String name);

        void Undo() override;
        void Redo() override;

        [[nodiscard]] String GetName() const override { return m_Name; }

    private:
        UUID m_Entity;
        String m_Snapshot;
        String m_Name;
    };

    //<--- Several commands the user sees as one action, like the entity created by "Create Parent"
    //together with the reparenting that follows it ---<<
    class CompositeCommand final : public UndoCommand
    {
    public:
        explicit CompositeCommand(String name) : m_Name(Move(name)) { }

        void Add(const Ref<UndoCommand>& command);
        [[nodiscard]] bool IsEmpty() const { return m_Commands.empty(); }

        void Undo() override;
        void Redo() override;

        [[nodiscard]] String GetName() const override { return m_Name; }

    private:
        Vector<Ref<UndoCommand>> m_Commands;
        String m_Name;
    };

    //<--- An entity the editor has just put in the scene ---<<
    void RecordEntityCreated(Entity entity, const String& name);
    //<--- An entity the editor is about to take out of it. Call it while the entity is still whole ---<<
    void RecordEntityDestroyed(Entity entity, const String& name);

    //<--- Captures an entity, and pushes whatever changed about it when it goes out of scope. Wrap
    //it around a piece of editor code that touches an entity and that code becomes undoable ---<<
    class ScopedEntityEdit
    {
    public:
        ScopedEntityEdit(Entity entity, String name);
        ~ScopedEntityEdit();

        ScopedEntityEdit(const ScopedEntityEdit&) = delete;
        ScopedEntityEdit& operator=(const ScopedEntityEdit&) = delete;

        //<--- Pushes early, for callers that go on to do something else in the same scope ---<<
        void Commit();

    private:
        Entity m_Entity;
        String m_Before;
        String m_Name;
    };
}
