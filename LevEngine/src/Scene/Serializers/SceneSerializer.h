#pragma once
#include "Scene/Scene.h"
#include <yaml-cpp/yaml.h>

namespace LevEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		~SceneSerializer() = default;

		void SerializeEntity(YAML::Emitter& out, Entity entity) const;
		void Serialize(const String& filepath) const;
		void SerializeRuntime(const String& filepath);

		bool Deserialize(const String& filepath);
		bool DeserializeRuntime(const String& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

