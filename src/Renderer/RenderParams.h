#pragma once
#include "Camera/SceneCamera.h"

struct RenderParams
{
    Vector3 CameraPosition;
    SceneCamera& Camera;
    Matrix& CameraViewMatrix;
    Matrix& CameraPerspectiveViewProjectionMatrix;
};
