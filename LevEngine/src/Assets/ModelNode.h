#pragma once

namespace LevEngine
{
    struct ModelNode
    {
        UUID MeshUUID{};
        Matrix Transform{};
        String Name{};
        Vector<ModelNode*> Children{};
    };
}