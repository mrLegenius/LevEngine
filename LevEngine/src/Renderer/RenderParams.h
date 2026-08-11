#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class SceneCamera;

    struct LEV_API RenderParams
    {
        SceneCamera* Camera;
        Vector3 CameraPosition;
        Matrix CameraViewMatrix;
        Matrix CameraPerspectiveViewProjectionMatrix;
    };
}
