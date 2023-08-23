#pragma once
#include "Camera/SceneCamera.h"
#include "Math/Math.h"

namespace LevEngine
{
struct RenderParams
{
	SceneCamera& Camera;
	Vector3 CameraPosition;
	Matrix CameraViewMatrix;
	Matrix CameraPerspectiveViewProjectionMatrix;
};
}