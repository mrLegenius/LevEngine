#pragma once
#include "Assets.h"
#include "Scene/Entity.h"
struct Prefabs
{
    static Entity Log(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("Log");
        auto mesh = scene->CreateEntity("LogMesh");

        mesh.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>());
        mesh.GetComponent<Transform>().SetPositionY(-0.5f);
        auto& meshRenderer = mesh.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), MeshAssets::Log());
        meshRenderer.diffuseTexture = TextureAssets::Log();
        entity.AddComponent<BoxCollider>(Vector3(1, 2, 1));
        auto& transform = entity.GetComponent<Transform>();
        auto& rigidbody = entity.AddComponent<Rigidbody>();

        transform.SetLocalScale(Vector3::One * 5.0f);
        rigidbody.gravityScale = 1;
        rigidbody.mass = 1.0f;
        rigidbody.InitCubeInertia(transform);
        return entity;
    }

    static Entity Gear(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("Gear");
        entity.AddComponent<BoxCollider>(Vector3(0.9f, 0.1f, 0.9f));
        auto& mesh = entity.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), MeshAssets::Gear());
        mesh.diffuseTexture = TextureAssets::Gear();
        auto& rb = entity.AddComponent<Rigidbody>();
        auto& transform = entity.GetComponent<Transform>();

        transform.SetLocalScale(Vector3::One * 0.1f);
        rb.gravityScale = 1;
        rb.mass = 5.0f;
        rb.InitCubeInertia(transform);

        return entity;
    }

    static Entity Rock(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("Rock");
        auto mesh = scene->CreateEntity("RockMesh");

        mesh.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>());
        mesh.GetComponent<Transform>().SetLocalScale(Vector3::One * 0.075f / 1.5f);
        auto& meshRenderer = mesh.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), MeshAssets::Rock());
        meshRenderer.diffuseTexture = TextureAssets::Rock();

        constexpr auto colliderScale = 3.25f * 1.5f;
        entity.AddComponent<BoxCollider>(Vector3(colliderScale, colliderScale, colliderScale));
        auto& rb = entity.AddComponent<Rigidbody>();
        auto& transform = entity.GetComponent<Transform>();

        transform.SetLocalScale(Vector3::One * 1.5f);
        rb.gravityScale = 1;
        rb.mass = 100.0f;
        rb.InitCubeInertia(transform);
        return entity;
    }

    static Entity LavaRock(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("LavaRock");
        auto mesh = scene->CreateEntity("LavaRockMesh");

        mesh.GetComponent<Transform>().SetParent(&entity.GetComponent<Transform>());
        mesh.GetComponent<Transform>().SetLocalScale(Vector3::One * 0.2f);
        mesh.GetComponent<Transform>().SetLocalRotation(Vector3{90.0f, 0.0f, -90.0f});
        auto& meshRenderer = mesh.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), LavaRockAssets::Mesh());
        meshRenderer.ambientTexture = LavaRockAssets::AmbientTexture();
        meshRenderer.diffuseTexture = LavaRockAssets::AmbientTexture();
        meshRenderer.emissiveTexture = LavaRockAssets::EmissiveTexture();
        meshRenderer.specularTexture = LavaRockAssets::SpecularTexture();
        meshRenderer.normalTexture = LavaRockAssets::NormalTexture();

        constexpr auto colliderScale = 2.0f;
        entity.AddComponent<BoxCollider>(Vector3(colliderScale, colliderScale, colliderScale));
        auto& rb = entity.AddComponent<Rigidbody>();
        auto& transform = entity.GetComponent<Transform>();

        transform.SetLocalScale(Vector3::One);
        rb.gravityScale = 1;
        rb.mass = 100.0f;
        rb.InitCubeInertia(transform);
        return entity;
    }

    static Entity Sphere(const Ref<Scene> scene)
    {
        auto entity = scene->CreateEntity("Sphere");
        entity.AddComponent<SphereCollider>(2.0f);
        auto& mesh = entity.AddComponent<MeshRendererComponent>(ShaderAssets::Lit(), Mesh::CreateSphere(6));
        mesh.material = Materials::Gold();
        mesh.diffuseTexture = TextureAssets::Bricks();

        auto& rb = entity.AddComponent<Rigidbody>();
        auto& transform = entity.GetComponent<Transform>();

        transform.SetLocalScale(Vector3::One * 2.0f);
        rb.enabled = false;
        rb.gravityScale = 5;
        rb.mass = 2.0f;
        rb.InitCubeInertia(transform);

        return entity;
    }
};
