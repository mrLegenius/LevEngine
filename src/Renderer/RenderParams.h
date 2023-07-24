#pragma once
#include "Camera/SceneCamera.h"
namespace LevEngine
{
struct RenderParams
{
    Vector3 CameraPosition;
    SceneCamera& Camera;
    Matrix& CameraViewMatrix;
    Matrix& CameraPerspectiveViewProjectionMatrix;
};
}