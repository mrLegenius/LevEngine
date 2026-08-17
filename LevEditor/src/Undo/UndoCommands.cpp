#include "pch.h"
#include "UndoCommands.h"

#include "EntitySnapshot.h"
#include "UndoSystem.h"
#include "EntitySelection.h"

namespace LevEngine::Editor
{
    namespace
    {
        //<--- Nothing may keep pointing at an entity that is about to be destroyed ---<<
        void DeselectIfSelected(const Entity entity)
        {
            const auto selection = Selection::CurrentAs<EntitySelection>();

            if (selection && selection->Get() == entity)
                Selection::Deselect();
        }

        Entity FindEntity(const UUID uuid)
        {
            const auto& scene = SceneManager::GetActiveScene();
            if (!scene) return {};

            return scene->GetEntityByUUID(uuid);
        }
    }

    void EntityStateCommand::Apply(const String& state) const
    {
        const Entity entity = FindEntity(m_Entity);

        if (!entity)
        {
            Log::CoreWarning("Nothing to apply '{0}' to, the entity is gone", m_Name);
            return;
        }

        EntitySnapshot::Restore(entity, state);
    }

    EntityCreatedCommand::EntityCreatedCommand(const Entity entity, String name)
        : m_Entity(entity ? entity.GetUUID() : UUID(0))
        , m_Snapshot(EntitySnapshot::CaptureHierarchy(entity))
        , m_Name(Move(name))
    {
    }

    void EntityCreatedCommand::Undo()
    {
        const Entity entity = FindEntity(m_Entity);
        if (!entity) return;

        //<--- Captured again, so whatever was done to the entity after it was created survives a
        //redo ---<<
        m_Snapshot = EntitySnapshot::CaptureHierarchy(entity);

        DeselectIfSelected(entity);
        Scene::DestroyEntity(entity);
    }

    void EntityCreatedCommand::Redo()
    {
        const auto& scene = SceneManager::GetActiveScene();
        if (!scene) return;

        EntitySnapshot::RestoreHierarchy(scene, m_Snapshot);
    }

    EntityDestroyedCommand::EntityDestroyedCommand(const Entity entity, String name)
        : m_Entity(entity ? entity.GetUUID() : UUID(0))
        , m_Snapshot(EntitySnapshot::CaptureHierarchy(entity))
        , m_Name(Move(name))
    {
    }

    void EntityDestroyedCommand::Undo()
    {
        const auto& scene = SceneManager::GetActiveScene();
        if (!scene) return;

        EntitySnapshot::RestoreHierarchy(scene, m_Snapshot);
    }

    void EntityDestroyedCommand::Redo()
    {
        const Entity entity = FindEntity(m_Entity);
        if (!entity) return;

        DeselectIfSelected(entity);
        Scene::DestroyEntity(entity);
    }

    void CompositeCommand::Add(const Ref<UndoCommand>& command)
    {
        if (!command) return;

        m_Commands.push_back(command);
    }

    void CompositeCommand::Undo()
    {
        //<--- Backwards: the last thing done is the first thing taken back ---<<
        for (auto it = m_Commands.rbegin(); it != m_Commands.rend(); ++it)
            (*it)->Undo();
    }

    void CompositeCommand::Redo()
    {
        for (const auto& command : m_Commands)
            command->Redo();
    }

    void RecordEntityCreated(const Entity entity, const String& name)
    {
        if (!entity || !UndoSystem::IsRecording()) return;

        UndoSystem::Push(CreateRef<EntityCreatedCommand>(entity, name));
    }

    void RecordEntityDestroyed(const Entity entity, const String& name)
    {
        if (!entity || !UndoSystem::IsRecording()) return;

        UndoSystem::Push(CreateRef<EntityDestroyedCommand>(entity, name));
    }

    ScopedEntityEdit::ScopedEntityEdit(const Entity entity, String name)
        : m_Entity(entity)
        , m_Before(UndoSystem::IsRecording() ? EntitySnapshot::Capture(entity) : String{})
        , m_Name(Move(name))
    {
    }

    ScopedEntityEdit::~ScopedEntityEdit()
    {
        Commit();
    }

    void ScopedEntityEdit::Commit()
    {
        if (!m_Entity || m_Before.empty() || !UndoSystem::IsRecording()) return;

        const Entity entity = m_Entity;
        m_Entity = Entity{};

        String after = EntitySnapshot::Capture(entity);
        if (after == m_Before) return;

        UndoSystem::Push(CreateRef<EntityStateCommand>(entity.GetUUID(), m_Before, Move(after), m_Name));
    }
}
