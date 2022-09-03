#pragma once

#include "Scene/Components.h"
#include "Renderer/Camera/EditorCamera.h"

namespace LevEngine
{
    class Renderer3D
    {
    public:
        static void Init();

        static void DrawMesh(const glm::mat4& transform, const MeshRendererComponent& meshRenderer, int entityID);
        static void DrawSkybox(const LevEngine::SkyboxRendererComponent &skyboxRenderer, int entityID);

        static void SetLights(const glm::vec3 &dirLightDirection, const DirectionalLightComponent &dirLight);

        static void BeginScene(const Camera& camera, const glm::mat4& transform, const glm::vec3& position);
        static void BeginScene(const LevEngine::EditorCamera &camera);
        static void EndScene();

        static void Shutdown();


    };
}
