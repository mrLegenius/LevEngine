#include "levpch.h"
#include "SerializerUtils.h"

#include "Scene/Components/Components.h"
#include "Scene/Components/ComponentSerializer.h"
#include "Scene/Components/Transform/Transform.h"

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

	YAML::Emitter& operator<<(YAML::Emitter& out, const eastl::string& s)
	{
		out << s.c_str();
		return out;
	}

	void SerializeAsset(YAML::Emitter& out, const String& nodeName, const Ref<Asset>& asset)
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

	YAML::Node LoadYAMLFile(const Path& filepath)
	{
		return YAML::LoadFile(filepath.string());
	}

	bool LoadYAMLFileSafe(const Path& filepath, YAML::Node& node)
	{
		try
		{
			node = LoadYAMLFile(filepath);
			return true;
		}
		catch (std::exception& e)
		{
			Log::CoreError("Failed to load yaml file at {0}. Error: {1}", filepath.generic_string(), e.what());
			return false;
		}
	}

	void SerializeEntity(YAML::Emitter& out, const Entity entity)
	{
		LEV_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap;

		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().tag.c_str();

		if (const auto parent = entity.GetComponent<Transform>().GetParent())
			out << YAML::Key << "Parent" << YAML::Value << parent.GetUUID();

		for (const auto serializer : ClassCollection<IComponentSerializer>::Instance())
			serializer->Serialize(out, entity);

		out << YAML::EndMap;
	}
}

YAML::Node YAML::convert<Vector2>::encode(const Vector2& rhs)
{
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<Vector2>::decode(const Node& node, Vector2& rhs)
{
	if (!node.IsSequence() || node.size() != 2)
		return false;

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	return true;
}

YAML::Node YAML::convert<Vector3>::encode(const Vector3& rhs)
{
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.push_back(rhs.z);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<Vector3>::decode(const Node& node, Vector3& rhs)
{
	if (!node.IsSequence() || node.size() != 3)
		return false;

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	rhs.z = node[2].as<float>();
	return true;
}

YAML::Node YAML::convert<Vector4>::encode(const Vector4& rhs)
{
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.push_back(rhs.z);
	node.push_back(rhs.w);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<Vector4>::decode(const Node& node, Vector4& rhs)
{
	if (!node.IsSequence() || node.size() != 4)
		return false;

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	rhs.z = node[2].as<float>();
	rhs.w = node[3].as<float>();
	return true;
}

YAML::Node YAML::convert<LevEngine::Color>::encode(const LevEngine::Color& rhs)
{
	Node node;
	node.push_back(rhs.r);
	node.push_back(rhs.g);
	node.push_back(rhs.b);
	node.push_back(rhs.a);
	node.SetStyle(EmitterStyle::Flow);
	return node;
}

bool YAML::convert<LevEngine::Color>::decode(const Node& node, LevEngine::Color& rhs)
{
	if (!node.IsSequence() || node.size() != 4)
		return false;

	rhs.r = node[0].as<float>();
	rhs.g = node[1].as<float>();
	rhs.b = node[2].as<float>();
	rhs.a = node[3].as<float>();
	return true;
}

YAML::Node YAML::convert<eastl::basic_string<char>>::encode(const eastl::string& rhs)
{
	Node node;
	const std::string str = rhs.c_str();
	node.push_back(str);
	return node;
}

bool YAML::convert<eastl::basic_string<char>>::decode(const Node& node, eastl::string& rhs)
{
	rhs = node.as<std::string>().c_str();
	return true;
}
