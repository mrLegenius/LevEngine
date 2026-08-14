#include "levpch.h"
#include "PlanetOceanPass.h"

#include "PlanetShaders.h"
#include "Math/BoundingVolume.h"
#include "Planet/PlanetChunk.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderType.h"
#include "Renderer/3D/Mesh.h"
#include "Scene/Components/Planet/Planet.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	PlanetOceanPass::PlanetOceanPass(const Ref<PipelineState>& pipelineState)
		: m_PipelineState(pipelineState) { }

	String PlanetOceanPass::PassName() { return "Planet Ocean"; }

	bool PlanetOceanPass::Begin(entt::registry& registry, RenderParams& params)
	{
		m_PipelineState->Bind();

		return RenderPass::Begin(registry, params);
	}

	void PlanetOceanPass::Process(entt::registry& registry, RenderParams& params)
	{
		LEV_PROFILE_FUNCTION();

		const auto planets = registry.group<>(entt::get<Transform, PlanetComponent>);
		if (planets.begin() == planets.end()) return;

		const Ref<Shader> passShader = m_PipelineState->GetShader(ShaderType::Vertex);
		const Ref<Shader> oceanShader = PlanetShaders::Ocean();

		if (!oceanShader) return;

		Ref<Shader> boundShader = passShader;
		BindShader(oceanShader, boundShader);

		for (const auto entity : planets)
		{
			auto [transform, planet] = planets.get<Transform, PlanetComponent>(entity);

			if (!planet.Surface || !planet.Ocean.Render) continue;

			const Vector<PlanetChunk*>& chunks = planet.Surface->GetVisibleChunks();
			if (chunks.empty()) continue;

			const Matrix planetToWorld = transform.GetModel();

			m_Constants.Bind(planet, planetToWorld);
			const float maxScale = Math::MaxElement(transform.GetWorldScale());

			for (const PlanetChunk* chunk : chunks)
			{
				//<--- No water over ground that is entirely above sea level ---<<
				if (!chunk->HasWater()) continue;

				const Ref<Mesh>& mesh = chunk->GetMesh();
				if (!mesh || !mesh->IndexBuffer) continue;

				if (RenderSettings::UseFrustumCulling)
				{
					// The chunk's bounds cover its terrain, and the water sits inside them: sea level
					// is between the chunk's highest and lowest ground, or there would be no water
					// here at all. So the terrain's sphere serves for both.
					const Vector3 worldCenter = Vector3::Transform(chunk->GetBoundsCenter(), planetToWorld);
					const SphereBoundingVolume bounds(worldCenter, chunk->GetBoundsRadius() * maxScale);

					if (!bounds.IsOnFrustum(params.Camera->GetFrustum())) continue;
				}

				const Matrix chunkToWorld = Matrix::CreateTranslation(chunk->GetOrigin()) * planetToWorld;

				Renderer3D::DrawMesh(chunkToWorld, mesh, oceanShader);
			}

			m_Constants.Unbind();
		}

		BindShader(passShader, boundShader);
	}

	void PlanetOceanPass::End(entt::registry& registry, RenderParams& params)
	{
		m_PipelineState->Unbind();
	}
}
