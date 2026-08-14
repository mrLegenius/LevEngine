#include "levpch.h"
#include "PlanetPass.h"

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
	PlanetPass::PlanetPass(const Ref<PipelineState>& pipelineState, const bool deferred)
		: m_PipelineState(pipelineState), m_Deferred(deferred) { }

	String PlanetPass::PassName() { return "Planet Surface"; }

	bool PlanetPass::Begin(entt::registry& registry, RenderParams& params)
	{
		m_PipelineState->Bind();

		return RenderPass::Begin(registry, params);
	}

	void PlanetPass::Process(entt::registry& registry, RenderParams& params)
	{
		LEV_PROFILE_FUNCTION();

		const auto planets = registry.group<>(entt::get<Transform, PlanetComponent>);
		if (planets.begin() == planets.end()) return;

		const Ref<Shader> passShader = m_PipelineState->GetShader(ShaderType::Vertex);
		const Ref<Shader> surfaceShader = m_Deferred ? PlanetShaders::SurfaceDeferred() : PlanetShaders::Surface();

		if (!surfaceShader) return;

		//<--- Begin() bound the pipeline, which bound its own shader ---<<
		Ref<Shader> boundShader = passShader;
		BindShader(surfaceShader, boundShader);

		for (const auto entity : planets)
		{
			auto [transform, planet] = planets.get<Transform, PlanetComponent>(entity);

			if (!planet.Surface) continue;

			const Vector<PlanetChunk*>& chunks = planet.Surface->GetVisibleChunks();
			if (chunks.empty()) continue;

			const Matrix planetToWorld = transform.GetModel();

			m_Constants.Bind(planet, planetToWorld);
			const float maxScale = Math::MaxElement(transform.GetWorldScale());

			for (const PlanetChunk* chunk : chunks)
			{
				const Ref<Mesh>& mesh = chunk->GetMesh();
				if (!mesh || !mesh->IndexBuffer) continue;

				if (RenderSettings::UseFrustumCulling)
				{
					// Culled by the chunk's own bounding sphere rather than through the mesh, because
					// the mesh's bounds are in the chunk's local space and its origin is not the
					// planet's -- so the transform the mesh would be tested against is not this one.
					const Vector3 worldCenter = Vector3::Transform(chunk->GetBoundsCenter(), planetToWorld);
					const SphereBoundingVolume bounds(worldCenter, chunk->GetBoundsRadius() * maxScale);

					if (!bounds.IsOnFrustum(params.Camera->GetFrustum())) continue;
				}

				// The chunk's origin goes into the model matrix, which is what keeps its vertices small
				// enough for a float to hold: see PlanetChunkGeometry::Origin.
				const Matrix chunkToWorld = Matrix::CreateTranslation(chunk->GetOrigin()) * planetToWorld;

				Renderer3D::DrawMesh(chunkToWorld, mesh, surfaceShader);
			}

			m_Constants.Unbind();
		}

		//<--- End() unbinds the pipeline, so leave its own shader as the bound one ---<<
		BindShader(passShader, boundShader);
	}

	void PlanetPass::End(entt::registry& registry, RenderParams& params)
	{
		m_PipelineState->Unbind();
	}
}
