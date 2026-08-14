#include "levpch.h"
#include "PlanetGeometry.h"

namespace LevEngine
{
	namespace
	{
		// Half the angle a cube face subtends. A face coordinate of ±1 is 45 degrees off the face's
		// axis, which is what the tangent below has to map to.
		constexpr float k_FaceHalfAngle = Math::Pi / 4.0f;

		const PlanetFaceBasis k_FaceBases[k_PlanetFaceCount] =
		{
			//<--- Right, Up, Forward. Right cross Up is Forward for every one of them ---<<
			{{0, 0, -1}, {0, 1, 0}, {1, 0, 0}},  //<--- +X ---<<
			{{0, 0, 1}, {0, 1, 0}, {-1, 0, 0}},  //<--- -X ---<<
			{{1, 0, 0}, {0, 0, -1}, {0, 1, 0}},  //<--- +Y, the north pole ---<<
			{{1, 0, 0}, {0, 0, 1}, {0, -1, 0}},  //<--- -Y ---<<
			{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},   //<--- +Z ---<<
			{{-1, 0, 0}, {0, 1, 0}, {0, 0, -1}}, //<--- -Z ---<<
		};
	}

	PlanetFaceBasis PlanetGeometry::GetFaceBasis(const PlanetFace face)
	{
		return k_FaceBases[static_cast<uint32_t>(face)];
	}

	Vector3 PlanetGeometry::FaceToDirection(const PlanetFace face, const Vector2 faceCoordinates)
	{
		const PlanetFaceBasis basis = GetFaceBasis(face);

		//<--- [0, 1] across the face to [-1, 1] either side of its axis ---<<
		const float u = faceCoordinates.x * 2.0f - 1.0f;
		const float v = faceCoordinates.y * 2.0f - 1.0f;

		// The tangent adjustment. tan(u * 45 degrees) is the cube coordinate whose direction is
		// u * 45 degrees off axis, so equal steps in u become equal steps in angle rather than equal
		// steps along the flat face -- which is where the stretching came from.
		const float x = std::tan(u * k_FaceHalfAngle);
		const float y = std::tan(v * k_FaceHalfAngle);

		Vector3 direction = basis.Forward + basis.Right * x + basis.Up * y;
		direction.Normalize();

		return direction;
	}

	PlanetFace PlanetGeometry::DirectionToFace(const Vector3 direction, Vector2& outFaceCoordinates)
	{
		//<--- The face is whichever axis the direction leans on hardest ---<<
		const float absX = Math::Abs(direction.x);
		const float absY = Math::Abs(direction.y);
		const float absZ = Math::Abs(direction.z);

		PlanetFace face;
		float major;

		if (absX >= absY && absX >= absZ)
		{
			face = direction.x >= 0.0f ? PlanetFace::PositiveX : PlanetFace::NegativeX;
			major = absX;
		}
		else if (absY >= absZ)
		{
			face = direction.y >= 0.0f ? PlanetFace::PositiveY : PlanetFace::NegativeY;
			major = absY;
		}
		else
		{
			face = direction.z >= 0.0f ? PlanetFace::PositiveZ : PlanetFace::NegativeZ;
			major = absZ;
		}

		if (major < Math::FloatEpsilon)
		{
			outFaceCoordinates = Vector2(0.5f, 0.5f);
			return face;
		}

		// Project onto the face plane to recover the cube coordinates, then undo the tangent. Note
		// the basis vectors are unit and orthogonal, so the projection is two dot products.
		const PlanetFaceBasis basis = GetFaceBasis(face);
		const Vector3 onFace = direction / major;

		const float x = onFace.Dot(basis.Right);
		const float y = onFace.Dot(basis.Up);

		const float u = std::atan(x) / k_FaceHalfAngle;
		const float v = std::atan(y) / k_FaceHalfAngle;

		outFaceCoordinates = Vector2(u * 0.5f + 0.5f, v * 0.5f + 0.5f);

		return face;
	}

	const char* PlanetGeometry::GetFaceName(const PlanetFace face)
	{
		switch (face)
		{
		case PlanetFace::PositiveX: return "+X";
		case PlanetFace::NegativeX: return "-X";
		case PlanetFace::PositiveY: return "+Y";
		case PlanetFace::NegativeY: return "-Y";
		case PlanetFace::PositiveZ: return "+Z";
		case PlanetFace::NegativeZ: return "-Z";
		}

		return "Unknown";
	}
}
