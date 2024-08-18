#pragma once
#include "Math/Matrix.h"
#include "Math/Color.h"

namespace LevEngine
{
	class AssimpConverter
	{
	public:
		static Matrix ToMatrix(const aiMatrix4x4& aiMatrix, bool transpose)
		{
			Matrix matrix = Matrix(aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
			                     aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
			                     aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
			                     aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
			
			return transpose ? matrix.Transpose() : matrix;
		}

		static Vector3 ToVector3(const aiVector3D& aiVector)
		{
			return Vector3{ aiVector.x, aiVector.y, aiVector.z };
		}
		
		static Vector2 ToVector2(const aiVector3D& aiVector)
		{
			return Vector2{ aiVector.x, aiVector.y };
		}

		static Quaternion ToQuaternion(const aiQuaternion& aiQuaternion)
		{
			return Quaternion{ aiQuaternion.x, aiQuaternion.y, aiQuaternion.z, aiQuaternion.w };
		}

		static Color ToColor(const aiColor4D& color)
		{
			return Color(color.r, color.g, color.b, color.a);
		}

		static String ToName(const aiString& string)
		{
			constexpr uint32_t maxNameLength = 64;
			String name = String(string.C_Str()).substr(0, maxNameLength);

			std::ranges::replace(name, '\\', '_');
			std::ranges::replace(name, '/', '_');
			std::ranges::replace(name, ':', '_');
			std::ranges::replace(name, '?', '_');
			std::ranges::replace(name, '"', '_');
			std::ranges::replace(name, '<', '_');
			std::ranges::replace(name, '>', '_');
			std::ranges::replace(name, '|', '_');
			
			return name;
		}
	};
}
