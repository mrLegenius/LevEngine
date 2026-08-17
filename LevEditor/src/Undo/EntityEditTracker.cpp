#include "pch.h"
#include "EntityEditTracker.h"

#include "EntitySnapshot.h"
#include "UndoCommands.h"
#include "UndoSystem.h"

namespace LevEngine::Editor
{
    void EntityEditTracker::Reset()
    {
        m_Baseline.clear();
        m_Entity = UUID(0);
        m_HasInteracted = false;
    }

    void EntityEditTracker::Update(const Entity entity, const bool isInteracting, const bool isEngaged)
    {
        if (!entity || !UndoSystem::IsRecording())
        {
            Reset();
            return;
        }

        //<--- Another entity, or somebody else has recorded since: either way what was captured is
        //not what the scene came from anymore ---<<
        if (entity.GetUUID() != m_Entity || m_Revision != UndoSystem::GetRevision())
        {
            m_Entity = entity.GetUUID();
            m_Revision = UndoSystem::GetRevision();
            m_Baseline.clear();
            m_HasInteracted = false;
        }

        if (isInteracting)
        {
            //<--- The snapshot from before the widget was grabbed is the one to go back to, so
            //nothing is captured until it is let go ---<<
            m_HasInteracted = true;
            return;
        }

        if (m_HasInteracted)
        {
            m_HasInteracted = false;

            String after = EntitySnapshot::Capture(entity);

            if (!m_Baseline.empty() && after != m_Baseline)
            {
                UndoSystem::Push(CreateRef<EntityStateCommand>(entity.GetUUID(), m_Baseline, after,
                    Format("{0} {1}", m_ActionName, entity.GetName())));

                m_Revision = UndoSystem::GetRevision();
            }

            m_Baseline = Move(after);
            return;
        }

        if (isEngaged)
            m_Baseline = EntitySnapshot::Capture(entity);
        else
            m_Baseline.clear();
    }
}
