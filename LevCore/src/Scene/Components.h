#pragma once
#include <string>

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/Texture.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/Shader.h"
#include "Kernel/UUID.h"

namespace LevEngine
{
    struct IDComponent
    {
        UUID ID;

        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
    };

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		explicit TagComponent(std::string other)
			: tag(std::move(other)) { }
	};
	
	struct TransformComponent
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		explicit TransformComponent(const glm::vec3& position)
			: position(position) { }

		[[nodiscard]] glm::mat4 GetModel() const
		{
			return glm::translate(glm::mat4(1.0f), position) *
				glm::toMat4(glm::quat(rotation)) * 
				glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 color {1.0f};
        Ref<Texture2D> Texture;
        float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
	};

    struct CircleRendererComponent
    {
        glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
        float Thickness = 1.0f;
        float Fade = 0.005f;

        CircleRendererComponent() = default;
        CircleRendererComponent(const CircleRendererComponent&) = default;
    };

    struct MeshRendererComponent
    {
        Ref<Mesh> Mesh;
        Ref<Shader> Shader;
        Ref<Texture2D> Texture;

        MeshRendererComponent() = default;
        MeshRendererComponent(const MeshRendererComponent&) = default;
    };

    //TODO: Make this asset for camera instead of component
    struct SkyboxRendererComponent
    {
        Ref<TextureSkybox> Texture;
        Ref<Mesh> Mesh;

        SkyboxRendererComponent() = default;
        SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
    };

	struct CameraComponent
	{
		SceneCamera camera;
		bool isMain = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};


    class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* instance = nullptr;

		ScriptableEntity* (*InstantiateScript)() = nullptr;
		void (*DestroyScript)(NativeScriptComponent*) = nullptr;

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
		}
	};

    template<typename... Component>
    struct ComponentGroup { };

    using AllComponents = ComponentGroup<
            TransformComponent,
            SpriteRendererComponent,
            CircleRendererComponent,
            SkyboxRendererComponent,
            MeshRendererComponent,
            CameraComponent,
            NativeScriptComponent>;
}
