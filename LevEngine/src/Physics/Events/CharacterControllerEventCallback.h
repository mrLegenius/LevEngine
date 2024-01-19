#pragma once
#include "characterkinematic/PxController.h"

namespace LevEngine
{
    class CharacterControllerEventCallback : public physx::PxUserControllerHitReport
    {
    private:
         void onShapeHit(const physx::PxControllerShapeHit& hit) override;
         void onControllerHit(const physx::PxControllersHit& hit) override;
         void onObstacleHit(const physx::PxControllerObstacleHit& hit) override;
    };
}