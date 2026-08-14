#include "levpch.h"
#include "PlanetChunkBuilder.h"

#include "PlanetSampler.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/Pipeline/VertexBuffer.h"

namespace LevEngine
{
	namespace
	{
		// One ring of samples outside the chunk on every side. Border vertices need their neighbours
		// to get a normal by central difference, and taking those from outside the chunk is what makes
		// the shading continuous across a chunk boundary -- otherwise every chunk edge is a visible
		// crease, whether or not the geometry lines up.
		//
		// The ring falls outside the face's own [0, 1] range for chunks on a face edge, which is
		// harmless: a face coordinate past 1 still names a real direction, just past the cube's edge,
		// and the shape is a function of direction rather than of the face it was reached through.
		constexpr int32_t k_Border = 1;

		struct ExtendedGrid
		{
			int32_t Side = 0; //<--- Including the border ring on both sides ---<<
			Vector<Vector3> Directions;
			Vector<Vector3> Positions; //<--- Planet space, before Origin is subtracted ---<<
			Vector<PlanetSurfaceSample> Samples;

			[[nodiscard]] int32_t Index(const int32_t i, const int32_t j) const
			{
				return (j + k_Border) * Side + (i + k_Border);
			}
		};
	}

	PlanetChunkGeometry PlanetChunkBuilder::Build(const PlanetSampler& sampler, const PlanetFace face,
	                                             const Vector2 minCoordinates, const Vector2 maxCoordinates,
	                                             const uint32_t resolution, const float skirtDepth)
	{
		LEV_PROFILE_FUNCTION();

		PlanetChunkGeometry geometry;

		const auto quads = static_cast<int32_t>(Math::Max(1u, resolution));
		const int32_t side = quads + 1;

		const Vector2 size = maxCoordinates - minCoordinates;
		const Vector2 step = size / static_cast<float>(quads);

		//<--- Sample the grid and one ring around it ---<<
		ExtendedGrid grid;
		grid.Side = side + 2 * k_Border;

		const auto extendedCount = static_cast<size_t>(grid.Side) * static_cast<size_t>(grid.Side);
		grid.Directions.resize(extendedCount);
		grid.Positions.resize(extendedCount);
		grid.Samples.resize(extendedCount);

		float minElevation = std::numeric_limits<float>::max();
		float maxElevation = std::numeric_limits<float>::lowest();

		for (int32_t j = -k_Border; j < side + k_Border; ++j)
		{
			for (int32_t i = -k_Border; i < side + k_Border; ++i)
			{
				const Vector2 faceCoordinates
				{
					minCoordinates.x + step.x * static_cast<float>(i),
					minCoordinates.y + step.y * static_cast<float>(j),
				};

				const Vector3 direction = PlanetGeometry::FaceToDirection(face, faceCoordinates);
				const PlanetSurfaceSample sample = sampler.Sample(direction);

				const int32_t index = grid.Index(i, j);
				grid.Directions[index] = direction;
				grid.Samples[index] = sample;
				grid.Positions[index] = direction * (sampler.GetRadius() + sample.Shape.Elevation);

				//<--- The border ring is scaffolding for the normals; it is not part of the chunk ---<<
				if (i >= 0 && i < side && j >= 0 && j < side)
				{
					minElevation = Math::Min(minElevation, sample.Shape.Elevation);
					maxElevation = Math::Max(maxElevation, sample.Shape.Elevation);
				}
			}
		}

		geometry.MinElevation = minElevation;
		geometry.MaxElevation = maxElevation;

		// The middle of the chunk, which everything is measured from. Taken from the surface rather
		// than from the sphere so that a chunk on a mountain has its origin on the mountain, and its
		// local coordinates stay small.
		const Vector2 centerCoordinates = minCoordinates + size * 0.5f;
		const Vector3 centerDirection = PlanetGeometry::FaceToDirection(face, centerCoordinates);
		geometry.Origin = centerDirection * (sampler.GetRadius() + sampler.GetElevation(centerDirection));

		const auto vertexCount = static_cast<size_t>(side) * static_cast<size_t>(side);
		const auto skirtVertexCount = static_cast<size_t>(side) * 4;

		geometry.Positions.reserve(vertexCount + skirtVertexCount);
		geometry.Normals.reserve(vertexCount + skirtVertexCount);
		geometry.Tangents.reserve(vertexCount + skirtVertexCount);
		geometry.UVs.reserve(vertexCount + skirtVertexCount);
		geometry.Climate.reserve(vertexCount + skirtVertexCount);
		geometry.Indices.reserve(static_cast<size_t>(quads) * quads * 6 + skirtVertexCount * 6);

		float boundingRadiusSquared = 0.0f;

		for (int32_t j = 0; j < side; ++j)
		{
			for (int32_t i = 0; i < side; ++i)
			{
				const int32_t index = grid.Index(i, j);

				const Vector3 direction = grid.Directions[index];
				const PlanetSurfaceSample& sample = grid.Samples[index];

				// Normal from the neighbouring surface points rather than from the analytic gradient
				// of the noise: this is the normal of the triangles that actually get drawn, so the
				// lighting agrees with the silhouette. A gradient-derived normal describes the
				// continuous surface the mesh only approximates, and the difference shows up as
				// shading that slides as the LOD changes.
				const Vector3 alongU = grid.Positions[grid.Index(i + 1, j)] - grid.Positions[grid.Index(i - 1, j)];
				const Vector3 alongV = grid.Positions[grid.Index(i, j + 1)] - grid.Positions[grid.Index(i, j - 1)];

				Vector3 normal = alongU.Cross(alongV);
				normal.Normalize();

				//<--- Right cross Up is outward on every face, so this needs no per-face fixup ---<<
				Vector3 tangent = alongU;
				tangent.Normalize();

				//<--- Gram-Schmidt: the tangent has to lie in the surface for the TBN to be a frame ---<<
				tangent -= normal * normal.Dot(tangent);
				tangent.Normalize();

				const Vector3 localPosition = grid.Positions[index] - geometry.Origin;

				geometry.Positions.emplace_back(localPosition);
				geometry.Normals.emplace_back(normal);
				geometry.Tangents.emplace_back(tangent);

				geometry.UVs.emplace_back(Vector2
				{
					minCoordinates.x + step.x * static_cast<float>(i),
					minCoordinates.y + step.y * static_cast<float>(j),
				});

				//<--- Slope as one minus the cosine of the angle from straight up ---<<
				const float slope = Math::Saturate(1.0f - normal.Dot(direction));

				geometry.Climate.emplace_back(Vector4
				{
					sample.Shape.Elevation,
					sample.Climate.Temperature,
					sample.Climate.Humidity,
					slope,
				});

				boundingRadiusSquared = Math::Max(boundingRadiusSquared, localPosition.LengthSquared());
			}
		}

		//<--- Grid triangles. Right cross Up is outward, so (00, 10, 11) faces out ---<<
		for (int32_t j = 0; j < quads; ++j)
		{
			for (int32_t i = 0; i < quads; ++i)
			{
				const auto v00 = static_cast<uint32_t>(j * side + i);
				const auto v10 = static_cast<uint32_t>(j * side + i + 1);
				const auto v01 = static_cast<uint32_t>((j + 1) * side + i);
				const auto v11 = static_cast<uint32_t>((j + 1) * side + i + 1);

				geometry.Indices.emplace_back(v00);
				geometry.Indices.emplace_back(v10);
				geometry.Indices.emplace_back(v11);

				geometry.Indices.emplace_back(v00);
				geometry.Indices.emplace_back(v11);
				geometry.Indices.emplace_back(v01);
			}
		}

		if (skirtDepth > 0.0f)
		{
			// One wall per edge. Each is a copy of the edge's vertices pushed towards the planet's
			// centre; the vertex data is copied rather than recomputed so the wall's top edge is
			// exactly the chunk's edge and cannot itself crack.
			//
			// Winding is derived rather than guessed: with Right cross Up outward, the outward
			// direction of an edge is minus the coordinate that is constant along it, and each pair
			// of triangles below is ordered to face that way. A wall wound the wrong way is invisible
			// from outside, which is precisely where the crack is seen from.
			const auto gridVertexCount = static_cast<uint32_t>(vertexCount);

			struct Edge
			{
				bool AlongU;      //<--- The edge runs along u (bottom and top) or along v ---<<
				bool AtMaximum;   //<--- It is the far edge of the chunk rather than the near one ---<<
			};

			const Edge edges[4] = {{true, false}, {true, true}, {false, false}, {false, true}};

			uint32_t skirtBase = gridVertexCount;

			for (const Edge& edge : edges)
			{
				for (int32_t k = 0; k < side; ++k)
				{
					const int32_t i = edge.AlongU ? k : (edge.AtMaximum ? quads : 0);
					const int32_t j = edge.AlongU ? (edge.AtMaximum ? quads : 0) : k;

					const Vector2 faceCoordinates
					{
						minCoordinates.x + step.x * static_cast<float>(i),
						minCoordinates.y + step.y * static_cast<float>(j),
					};

					const int32_t index = grid.Index(i, j);
					const Vector3 direction = grid.Directions[index];
					const Vector3 localPosition = grid.Positions[index] - direction * skirtDepth - geometry.Origin;

					// Copied out before appending: the source is an element of the very vector being
					// appended to, and a reference into it is only valid until it grows. The capacity is
					// reserved above so it will not, but that is not a thing to leave depending on a
					// reserve call thirty lines away.
					const auto surfaceVertex = static_cast<uint32_t>(j * side + i);
					const Vector3 surfaceNormal = geometry.Normals[surfaceVertex];
					const Vector3 surfaceTangent = geometry.Tangents[surfaceVertex];
					const Vector4 surfaceClimate = geometry.Climate[surfaceVertex];

					geometry.Positions.emplace_back(localPosition);
					geometry.Normals.emplace_back(surfaceNormal);
					geometry.Tangents.emplace_back(surfaceTangent);
					geometry.UVs.emplace_back(faceCoordinates);
					geometry.Climate.emplace_back(surfaceClimate);

					boundingRadiusSquared = Math::Max(boundingRadiusSquared, localPosition.LengthSquared());
				}

				for (int32_t k = 0; k < quads; ++k)
				{
					const int32_t i0 = edge.AlongU ? k : (edge.AtMaximum ? quads : 0);
					const int32_t j0 = edge.AlongU ? (edge.AtMaximum ? quads : 0) : k;
					const int32_t i1 = edge.AlongU ? k + 1 : i0;
					const int32_t j1 = edge.AlongU ? j0 : k + 1;

					const auto surface0 = static_cast<uint32_t>(j0 * side + i0);
					const auto surface1 = static_cast<uint32_t>(j1 * side + i1);
					const uint32_t skirt0 = skirtBase + static_cast<uint32_t>(k);
					const uint32_t skirt1 = skirt0 + 1;

					if (edge.AlongU != edge.AtMaximum)
					{
						//<--- Bottom (v = 0) and right (u = max) walls share a winding ---<<
						geometry.Indices.emplace_back(surface0);
						geometry.Indices.emplace_back(skirt0);
						geometry.Indices.emplace_back(surface1);

						geometry.Indices.emplace_back(surface1);
						geometry.Indices.emplace_back(skirt0);
						geometry.Indices.emplace_back(skirt1);
					}
					else
					{
						//<--- Top (v = max) and left (u = 0) walls share the other ---<<
						geometry.Indices.emplace_back(surface0);
						geometry.Indices.emplace_back(surface1);
						geometry.Indices.emplace_back(skirt0);

						geometry.Indices.emplace_back(surface1);
						geometry.Indices.emplace_back(skirt1);
						geometry.Indices.emplace_back(skirt0);
					}
				}

				skirtBase += static_cast<uint32_t>(side);
			}
		}

		geometry.BoundingRadius = Math::Sqrt(boundingRadiusSquared);

		return geometry;
	}

	Ref<Mesh> PlanetChunkBuilder::CreateMesh(const PlanetChunkGeometry& geometry)
	{
		LEV_PROFILE_FUNCTION();

		if (geometry.Positions.empty() || geometry.Indices.empty()) return nullptr;

		auto mesh = CreateRef<Mesh>();

		Vector3 boundsMin = geometry.Positions[0];
		Vector3 boundsMax = geometry.Positions[0];

		for (size_t vertex = 0; vertex < geometry.Positions.size(); ++vertex)
		{
			mesh->AddVertex(geometry.Positions[vertex]);
			mesh->AddNormal(geometry.Normals[vertex]);
			mesh->AddTangent(geometry.Tangents[vertex]);
			mesh->AddUV(geometry.UVs[vertex]);

			boundsMin = Vector3::Min(boundsMin, geometry.Positions[vertex]);
			boundsMax = Vector3::Max(boundsMax, geometry.Positions[vertex]);
		}

		for (const uint32_t index : geometry.Indices)
			mesh->AddIndex(index);

		mesh->Init();

		// The climate goes in as a stream of its own rather than being folded into an existing
		// semantic. Mesh binds whatever streams the shader declares and ignores the rest, so the
		// shadow and depth passes -- which know nothing about climate -- draw these meshes unchanged.
		const auto climateBuffer = VertexBuffer::Create(
			&geometry.Climate[0].x,
			static_cast<uint32_t>(geometry.Climate.size()),
			sizeof(Vector4));

		mesh->AddVertexBuffer(BufferBinding(k_ClimateSemantic, 0), climateBuffer);

		mesh->SetAABBBoundingVolume(boundsMin, boundsMax);

		return mesh;
	}
}
