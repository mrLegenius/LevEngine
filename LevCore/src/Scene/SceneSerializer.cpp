#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

#include "Entity.h"
#include "Kernel/Asserts.h"
#include "Components.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
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
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
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
}

namespace LevEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	
	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	template<typename T, typename SerializationFunction>
	static void SerializeComponent(YAML::Emitter& out, const std::string& key, Entity entity, SerializationFunction serialize)
	{
		if (entity.HasComponent<T>())
		{
			T& component = entity.GetComponent<T>();

			out << YAML::Key << key.c_str();
			out << YAML::BeginMap;
			
			serialize(component);

			out << YAML::EndMap;
		}
	}

	
	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
        LEV_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap;

		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		SerializeComponent<TagComponent>(out, "TagComponent", entity, 
			[&entity, &out](TagComponent& component)
			{
				out << YAML::Key << "Tag" << YAML::Value << component.tag;
			});

		SerializeComponent<TransformComponent>(out, "TransformComponent", entity, 
			[&entity, &out](TransformComponent& component)
			{
				out << YAML::Key << "Position" << YAML::Value << component.position;
				out << YAML::Key << "Rotation" << YAML::Value << component.rotation;
				out << YAML::Key << "Scale" << YAML::Value << component.scale;
			});

		SerializeComponent<CameraComponent>(out, "CameraComponent", entity,
			[&entity, &out](CameraComponent& component)
			{
				auto& camera = component.camera;
				out << YAML::Key << "Camera" << YAML::Value;
				out << YAML::BeginMap;
				out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.GetProjectionType());

				out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetFieldOfView();
				out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNear();
				out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFar();

				out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
				out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNear();
				out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFar();
				out << YAML::EndMap;

				out << YAML::Key << "Main" << YAML::Value << component.isMain;
				out << YAML::Key << "FixedAspectRatio" << YAML::Value << component.fixedAspectRatio;
			});

		SerializeComponent<SpriteRendererComponent>(out, "SpriteRendererComponent", entity,
			[&entity, &out](SpriteRendererComponent& component)
			{
				out << YAML::Key << "Color" << YAML::Value << component.color;
                //TODO: serialize texture by uuid
                if (component.Texture)
                    out << YAML::Key << "TexturePath" << YAML::Value << component.Texture->GetPath();

                out << YAML::Key << "TilingFactor" << YAML::Value << component.TilingFactor;
			});

        SerializeComponent<CircleRendererComponent>(out, "CircleRendererComponent", entity,
            [&entity, &out](CircleRendererComponent& component)
            {
                out << YAML::Key << "Color" << YAML::Value << component.Color;
                out << YAML::Key << "Thickness" << YAML::Value << component.Thickness;
                out << YAML::Key << "Fade" << YAML::Value << component.Fade;
            });

        SerializeComponent<SkyboxRendererComponent>(out, "SkyboxRendererComponent", entity,
                                                    [&entity, &out](SkyboxRendererComponent& component)
        {
            for (int i = 0; i < 6; ++i)
            {
                std::string texture = "Texture" + std::to_string(i);
                out << YAML::Key << texture << YAML::Value << component.Texture->GetPath(i);
            }
        });

        SerializeComponent<DirectionalLightComponent>(out, "DirectionalLightComponent", entity,
                                                  [&entity, &out](DirectionalLightComponent& component)
        {
            out << YAML::Key << "Ambient" << YAML::Value << component.Ambient;
            out << YAML::Key << "Diffuse" << YAML::Value << component.Diffuse;
            out << YAML::Key << "Specular" << YAML::Value << component.Specular;
        });

		out << YAML::EndMap;
	}
	
	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		//TODO: Put actual scene name
		out << YAML::Key << "Scene" << YAML::Value << "Scene Name"; 
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			const Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
		
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		LEV_NOT_IMPLEMENTED
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filepath);
        }
        catch (YAML::ParserException& e)
        {
            Log::CoreWarning(e.what());
            return false;
        }

		if (!data["Scene"])
			return false;

		auto sceneName = data["Scene"].as<std::string>();
		Log::Trace("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				auto uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				Log::Trace("Deserializing entity with ID = {0}, name = {1}", uuid, name);
				
				Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if(transformComponent)
				{
					auto& component = deserializedEntity.GetComponent<TransformComponent>();
					component.position = transformComponent["Position"].as<glm::vec3>();
					component.rotation = transformComponent["Rotation"].as<glm::vec3>();
					component.scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& component = deserializedEntity.AddComponent<CameraComponent>();
					auto& camera = component.camera;
					auto cameraProps = cameraComponent["Camera"];

					component.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					component.isMain = cameraComponent["Main"].as<bool>();
					camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraProps["ProjectionType"].as<int>()));

					camera.SetPerspective(
						cameraProps["PerspectiveFOV"].as<float>(),
						cameraProps["PerspectiveNear"].as<float>(),
						cameraProps["PerspectiveFar"].as<float>());

					camera.SetOrthographic(
						cameraProps["OrthographicSize"].as<float>(),
						cameraProps["OrthographicNear"].as<float>(),
						cameraProps["OrthographicFar"].as<float>());
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& component = deserializedEntity.AddComponent<SpriteRendererComponent>();

					component.color = spriteRendererComponent["Color"].as<glm::vec4>();
                    //TODO: deserialize texture by uuid
                    if (spriteRendererComponent["TexturePath"])
                        component.Texture = Texture2D::Create(spriteRendererComponent["TexturePath"].as<std::string>());

                    if (spriteRendererComponent["TilingFactor"])
                        component.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

                auto circleRendererComponent = entity["CircleRendererComponent"];
                if (circleRendererComponent)
                {
                    auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
                    crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
                    crc.Thickness = circleRendererComponent["Thickness"].as<float>();
                    crc.Fade = circleRendererComponent["Fade"].as<float>();
                }

                auto skyboxRendererComponent = entity["SkyboxRendererComponent"];
                if (skyboxRendererComponent)
                {
                    auto& crc = deserializedEntity.AddComponent<SkyboxRendererComponent>();
                    std::string paths[6];
                    for (int i = 0; i < 6; ++i)
                    {
                        std::string texture = "Texture" + std::to_string(i);
                        paths[i] = skyboxRendererComponent[texture].as<std::string>();
                    }

                    crc.Texture = TextureSkybox::Create(paths);
                }

                auto directionalLightComponent = entity["DirectionalLightComponent"];
                if (directionalLightComponent)
                {
                    auto& crc = deserializedEntity.AddComponent<DirectionalLightComponent>();
                    crc.Ambient = directionalLightComponent["Ambient"].as<glm::vec3>();
                    crc.Diffuse = directionalLightComponent["Diffuse"].as<glm::vec3>();
                    crc.Specular = directionalLightComponent["Specular"].as<glm::vec3>();
                }
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		LEV_NOT_IMPLEMENTED
	
		return false;
	}
}
