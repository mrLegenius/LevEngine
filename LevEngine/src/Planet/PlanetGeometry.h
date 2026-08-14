#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"

namespace LevEngine
{
	// The cube-sphere: six square grids inflated onto a sphere.
	//
	// A planet has to be cut into pieces that can be subdivided independently, and a sphere has no
	// natural grid. A latitude/longitude grid crowds towards the poles and has a seam at the date
	// line; an icosphere subdivides evenly but its quads are not square, so a chunk cannot be a
	// simple grid of vertices. Six cube faces have neither problem: each face is an ordinary square
	// grid a quadtree can split, and the six of them cover the sphere with only their edges to match
	// up -- and those match up automatically, because two chunks meeting at a cube edge compute the
	// same direction for the vertices they share.
	enum class PlanetFace : uint8_t
	{
		PositiveX = 0,
		NegativeX = 1,
		PositiveY = 2,
		NegativeY = 3,
		PositiveZ = 4,
		NegativeZ = 5,
	};

	inline constexpr uint32_t k_PlanetFaceCount = 6;

	// The three axes of a face. Right and Up span it, Forward points out through its middle, and
	// Right cross Up is Forward for all six -- which is what makes one triangle winding correct
	// everywhere instead of three faces coming out inside-out.
	struct PlanetFaceBasis
	{
		Vector3 Right;
		Vector3 Up;
		Vector3 Forward;
	};

	struct LEV_API PlanetGeometry
	{
		[[nodiscard]] static PlanetFaceBasis GetFaceBasis(PlanetFace face);

		// Face coordinates in [0, 1] to a direction on the unit sphere.
		//
		// Normalizing the cube point directly would work but wastes vertices: the projection stretches
		// the middle of a face and squeezes its corners, so a uniform grid on the cube becomes a
		// grid on the sphere whose cells differ in area by about 1.7 times. Running each coordinate
		// through a tangent first cancels most of that, which means a chunk's triangles are all
		// roughly the same size on screen and the LOD error metric means the same thing across a
		// whole face.
		[[nodiscard]] static Vector3 FaceToDirection(PlanetFace face, Vector2 faceCoordinates);

		//<--- Which face a direction belongs to, and where on it. Inverse of the above ---<<
		[[nodiscard]] static PlanetFace DirectionToFace(Vector3 direction, Vector2& outFaceCoordinates);

		[[nodiscard]] static const char* GetFaceName(PlanetFace face);
	};
}
