#pragma once

namespace LevEngine
{
    class SceneCamera;

    struct RenderParams
    {
        SceneCamera* Camera;
        Vector3 CameraPosition;
        Matrix CameraViewMatrix;
        Matrix CameraPerspectiveViewProjectionMatrix;
    };
}
