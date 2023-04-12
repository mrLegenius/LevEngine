#pragma once
#include "Camera/SceneCamera.h"

struct RenderParams
{
    SceneCamera& Camera;
    Matrix& CameraViewMatrix;
};
