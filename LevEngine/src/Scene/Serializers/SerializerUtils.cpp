#include "pch.h"
#include "SerializerUtils.h"
#include "Math/Math.h"

namespace LevEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const Color& c)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << c.r << c.g << c.b << c.a << YAML::EndSeq;
		return out;
	}
}
