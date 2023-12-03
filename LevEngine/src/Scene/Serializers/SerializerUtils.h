#pragma once

#include "Assets/AssetDatabase.h"
#include "Math/Math.h"
#include "Scene/Entity.h"

namespace YAML
{
	template<>
	struct convert<Vector2>
	{
		static Node encode(const Vector2& rhs);
		static bool decode(const Node& node, Vector2& rhs);
	};

	template<>
	struct convert<Vector3>
	{
		static Node encode(const Vector3& rhs);
		static bool decode(const Node& node, Vector3& rhs);
	};

	template<>
	struct convert<Vector4>
	{
		static Node encode(const Vector4& rhs);
		static bool decode(const Node& node, Vector4& rhs);
	};

	template<>
	struct convert<LevEngine::Color>
	{
		static Node encode(const LevEngine::Color& rhs);
		static bool decode(const Node& node, LevEngine::Color& rhs);
	};

	template<>
	struct convert<eastl::string>
	{
		static Node encode(const eastl::string& rhs);
		static bool decode(const Node& node, eastl::string& rhs);
	};
}

namespace LevEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Color& c);
	YAML::Emitter& operator<<(YAML::Emitter& out, const eastl::string& s);

	void SerializeAsset(YAML::Emitter& out, const String& nodeName, const Ref<Asset>& asset);

	template<class T>
	Ref<T> DeserializeAsset(const YAML::Node& node);

	template<class T>
	Ref<T> DeserializeAsset(YAML::Node&& node);

	void SerializeEntity(YAML::Emitter& out, Entity entity);
	YAML::Node LoadYAMLFile(const Path& filepath);
	bool LoadYAMLFileSafe(const Path& filepath, YAML::Node& node);
	
	template <typename T>
	bool TryParse(const YAML::Node& node, T& value);
}

#include "SerializerUtils.inl"
