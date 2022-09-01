#pragma once
#include "Texture.h"
#include "Camera/EditorCamera.h"
#include "Camera/Camera.h"
#include "Camera/OrthographicCamera.h"
#include "Scene/Components.h"

namespace LevEngine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		
		static void EndScene();
		
		static void Flush();

        static void DrawMesh(const glm::mat4 &transform, const MeshRendererComponent& meshRenderer, int entityID);
        static void DrawSkybox(const SkyboxRendererComponent &skyboxRenderer, int entityID);

		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& spriteRenderer, int entityID);
        static void DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness, float fade, int entityID);
        static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

        static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

        static float GetLineWidth();
        static void SetLineWidth(float width);

		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			[[nodiscard]] uint32_t GetTotalVertexCount() const { return quadCount * 4; }
			[[nodiscard]] uint32_t GetTotalIndexCount() const { return quadCount * 6; }
		};
		
		static void ResetStats();
		static Statistics GetStats();
	
	private:
		static void NextBatch();
		static void StartBatch();
		
		static void Draw(const glm::mat4& transform, const glm::vec4& color, const Ref<Texture2D>& texture, float tiling, int entityID = -1);
		static void AddQuadToVertexBuffer(const glm::mat4& model, const glm::vec4& color, const Ref<Texture2D>& texture, float tiling, int entityID = -1);
    };
}
