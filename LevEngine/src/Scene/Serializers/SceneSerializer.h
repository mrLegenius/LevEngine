#pragma once

#include "Kernel/Core.h"

namespace YAML
{
	class Emitter;
	class Node;
}
namespace LevEngine
{
	class Scene;

	class LEV_API SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		~SceneSerializer() = default;

		void Serialize(const Path& filepath) const;
		void SerializeRuntime(const String& filepath);

		bool Deserialize(const String& filepath) const;
		bool DeserializeRuntime(const String& filepath);

	private:
		
		void SerializeEntities(YAML::Emitter& out) const;
		void SerializeScriptSystems(YAML::Emitter& out) const;
		
		//<--- source is the scene file a missing reference is reported against ---<<
		void DeserializeEntities(const YAML::Node& data, const String& source) const;
		void DeserializeScriptSystems(const YAML::Node& data, const String& source) const;
		
	private:
		Ref<Scene> m_Scene;
	};
}

