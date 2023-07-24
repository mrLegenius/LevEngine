#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;
namespace LevEngine
{
struct Material
{
	Vector3 Emissive = Vector3{ 0, 0, 0 };
	Vector3 Ambient = Vector3{ 0, 0, 0 };
	Vector3 Diffuse = Vector3{ 0, 0, 0 };
	Vector3 Specular = Vector3{ 0, 0, 0 };

	float Shininess = 2;

	Material() = default;
	Material(const Vector3 ambient,
	         const Vector3 diffuse,
	         const Vector3 specular,
	         const float shininess)
	: Ambient(ambient), Diffuse(diffuse), Specular(specular), Shininess(shininess) { }
};
}
