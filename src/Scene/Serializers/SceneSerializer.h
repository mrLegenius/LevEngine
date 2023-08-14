#pragma once
#include "Scene/Scene.h"
#include "yaml-cpp/emitter.h"

namespace LevEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		~SceneSerializer() = default;

		void SerializeEntity(YAML::Emitter& out, Entity entity) const;
		void Serialize(const std::string& filepath) const;
		void SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

