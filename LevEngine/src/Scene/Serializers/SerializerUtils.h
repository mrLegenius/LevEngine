#pragma once

#include "Kernel/Core.h"

#include "Assets/AssetDatabase.h"
#include "Math/Math.h"
#include "Scene/Entity.h"

namespace YAML
{
	// These are explicit specializations, not templates: their members live in
	// SerializerUtils.cpp inside the engine and have to be exported like any other symbol.
	template<>
	struct convert<Vector2>
	{
		static LEV_API Node encode(const Vector2& rhs);
		static LEV_API bool decode(const Node& node, Vector2& rhs);
	};

	template<>
	struct convert<Vector3>
	{
		static LEV_API Node encode(const Vector3& rhs);
		static LEV_API bool decode(const Node& node, Vector3& rhs);
	};

	template<>
	struct convert<Vector4>
	{
		static LEV_API Node encode(const Vector4& rhs);
		static LEV_API bool decode(const Node& node, Vector4& rhs);
	};

	template<>
	struct convert<LevEngine::Color>
	{
		static LEV_API Node encode(const LevEngine::Color& rhs);
		static LEV_API bool decode(const Node& node, LevEngine::Color& rhs);
	};

	template<>
	struct convert<eastl::string>
	{
		static LEV_API Node encode(const eastl::string& rhs);
		static LEV_API bool decode(const Node& node, eastl::string& rhs);
	};
}

namespace LevEngine
{
	LEV_API YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v);
	LEV_API YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v);
	LEV_API YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v);
	LEV_API YAML::Emitter& operator<<(YAML::Emitter& out, const Color& c);
	LEV_API YAML::Emitter& operator<<(YAML::Emitter& out, const eastl::string& s);

	LEV_API void SerializeAsset(YAML::Emitter& out, const String& nodeName, const Ref<Asset>& asset);

	template<class T>
	Ref<T> DeserializeAsset(const YAML::Node& node);

	template<class T>
	Ref<T> DeserializeAsset(YAML::Node&& node);

	LEV_API void SerializeEntity(YAML::Emitter& out, Entity entity);
	LEV_API YAML::Node LoadYAMLFile(const Path& filepath);
	LEV_API bool LoadYAMLFileSafe(const Path& filepath, YAML::Node& node);

	template <typename T>
	void Write(YAML::Emitter& out, String key, T value);

	template <typename T>
	bool TryParse(const YAML::Node& node, T& value);
}

#include "SerializerUtils.inl"
