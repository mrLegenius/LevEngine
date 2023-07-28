#pragma once
#include "Camera/SceneCamera.h"
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