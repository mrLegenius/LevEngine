#pragma once

#include "Kernel/Core.h"
#include "characterkinematic/PxController.h"

namespace LevEngine
{
    class LEV_API CharacterControllerEventCallback : public physx::PxUserControllerHitReport
    {
         void onShapeHit(const physx::PxControllerShapeHit& hit) override;
         void onControllerHit(const physx::PxControllersHit& hit) override;
         void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;
    };
}