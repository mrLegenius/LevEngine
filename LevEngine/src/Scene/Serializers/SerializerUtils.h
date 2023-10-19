#pragma once
#include <yaml-cpp/yaml.h>

#include "Assets/Asset.h"
#include "Assets/AssetDatabase.h"
#include "Math/Color.h"

namespace YAML
{
	template<>
	struct convert<Vector2>
	{
		static Node encode(const Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vector3>
	{
		static Node encode(const Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Vector4>
	{
		static Node encode(const Vector4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Vector4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<LevEngine::Color>
	{
		static Node encode(const LevEngine::Color& rhs)
		{
			Node node;
			node.push_back(rhs.r);
			node.push_back(rhs.g);
			node.push_back(rhs.b);
			node.push_back(rhs.a);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, LevEngine::Color& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.r = node[0].as<float>();
			rhs.g = node[1].as<float>();
			rhs.b = node[2].as<float>();
			rhs.a = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<eastl::string>
	{
		static Node encode(const eastl::string& rhs)
		{
			Node node;
			const std::string str = rhs.c_str();
			node.push_back(str);
			return node;
		}

		static bool decode(const Node& node, eastl::string& rhs)
		{
			rhs = node.as<std::string>().c_str();
			return true;
		}
	};
}

namespace LevEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const Color& c);
	YAML::Emitter& operator<<(YAML::Emitter& out, const eastl::string& s);

	inline void SerializeAsset(YAML::Emitter& out, const String& nodeName, const Ref<Asset>& asset)
	{
		if (!asset) return;

		try
		{
			out << YAML::Key << nodeName.c_str() << YAML::Value << asset->GetUUID();
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to serialize asset {0}. {1}", nodeName, e.what());
		}
	}

	template<class T>
	Ref<T> DeserializeAsset(YAML::Node&& node)
	{
		static_assert(std::is_base_of_v<Asset, T>, "T must be Asset");

		if (!node) return nullptr;

		try
		{
			const UUID assetUUID = node.as<std::uint64_t>();
			return AssetDatabase::GetAsset<T>(assetUUID);
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to deserialize asset. {0}", e.what());
			return nullptr;
		}
	}

	void SerializeEntity(YAML::Emitter& out, Entity entity);
}
