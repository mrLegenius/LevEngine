#pragma once

namespace LevEngine
{
    class Scene;
}

namespace LevEngine::Editor
{
    //<--- An entity, or an entity and everything under it, written the way a scene file writes it.
    //Restoring goes back through the same serializers, so a component is handed its values by the
    //code path a scene load would use, rebuilds included ---<<
    class EntitySnapshot
    {
    public:
        //<--- One entity: its tag, its transform, its parent and every component it holds ---<<
        [[nodiscard]] static String Capture(Entity entity);
        //<--- The entity and its whole branch, parents before children ---<<
        [[nodiscard]] static String CaptureHierarchy(Entity entity);

        //<--- Puts a captured state back on an entity that is still there. Components the snapshot
        //does not know about are removed, which is what makes adding a component undoable ---<<
        static void Restore(Entity entity, const String& snapshot);

        //<--- Recreates a branch with the UUIDs it had, so whatever pointed at those entities still
        //finds them. Returns the entity the branch starts at ---<<
        static Entity RestoreHierarchy(const Ref<Scene>& scene, const String& snapshot);
    };
}
