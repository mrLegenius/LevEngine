#pragma once

#include "Kernel/Core.h"
#include "Scene/System.h"

namespace LevEngine
{
    // Drives the directional light of every moon from the starlight it reflects, so a night is lit by
    // whatever is actually up rather than by an authored ambient term. Runs after the suns have been
    // placed and coloured, because a moon's light is made out of theirs.
    class LEV_API MoonLightingSystem final : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override;

        // Same, without the frame, for the editor: a scene being edited renders its sky but runs no
        // systems, so the moonlight would otherwise stay whatever colour the light was authored with.
        static void Apply(entt::registry& registry);
    };
}
