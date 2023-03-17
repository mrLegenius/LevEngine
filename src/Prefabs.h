#pragma once
#include "Assets.h"
#include "Scene/Entity.h"
namespace Prefabs
{
	inline Entity& Log(const Ref<Scene> scene)
	{
        auto entity = scene->CreateEntity("Log");
        auto mesh = scene->CreateEntity("LogMesh");

        mesh.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>());
        mesh.GetComponent<Transform>().SetPositionY(-0.5f);
        mesh.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), MeshAssets::Log(), TextureAssets::Log());

        entity.AddComponent<BoxCollider>(Vector3(1, 2, 1));
        auto& transform = entity.GetComponent<Transform>();
        auto& rigidbody = entity.AddComponent<Rigidbody>();

        transform.SetLocalScale(Vector3::One * 5.0f);
        rigidbody.gravityScale = 10;
        rigidbody.mass = 1.0f;
        rigidbody.InitCubeInertia(transform);
        return entity;
	}

    inline Entity& Gear(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("Gear");
        entity.AddComponent<BoxCollider>(Vector3(0.9f, 0.1f, 0.9f));
        entity.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), MeshAssets::Gear(), TextureAssets::Gear());
        auto& rb = entity.AddComponent<Rigidbody>();
        auto& transform = entity.GetComponent<Transform>();

        transform.SetLocalScale(Vector3::One * 0.1f);
        rb.gravityScale = 10;
        rb.mass = 5.0f;
        rb.InitCubeInertia(transform);

        return entity;
    }

    inline Entity& Rock(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("Gear");
        entity.AddComponent<BoxCollider>(Vector3(3.25f, 3.25f, 3.25f));
        entity.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), MeshAssets::Rock(), TextureAssets::Rock());
        auto& rb = entity.AddComponent<Rigidbody>();
        auto& transform = entity.GetComponent<Transform>();

        transform.SetLocalScale(Vector3::One * 0.05f);
        rb.gravityScale = 10;
        rb.mass = 100.0f;
        rb.InitCubeInertia(transform);
        return entity;
    }
}
