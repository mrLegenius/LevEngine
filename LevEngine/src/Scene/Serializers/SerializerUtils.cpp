#include "levpch.h"
#include "SerializerUtils.h"
#include "Scene/Components/ComponentSerializer.h"

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
