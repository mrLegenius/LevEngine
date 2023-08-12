﻿#pragma once
#include "Components/ComponentSerializer.h"
#include "Components/TagSerializer.h"
#include "Scene/Scene.h"

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
		Ref<TagSerializer> m_TagSerializer;

		std::vector<Ref<IComponentSerializer>> m_Serializers;
	};
}

