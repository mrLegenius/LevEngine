#pragma once
#include "Scene/Scene.h"
#include <yaml-cpp/yaml.h>

#include "DataTypes/Path.h"

namespace LevEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		~SceneSerializer() = default;

		void Serialize(const Path& filepath) const;
		void SerializeRuntime(const String& filepath);

		bool Deserialize(const String& filepath) const;
		bool DeserializeRuntime(const String& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

