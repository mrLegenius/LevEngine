#pragma once
#include "Assets.h"
/*
namespace Prefabs
{
	inline std::shared_ptr<GameObject> Log(std::vector<std::shared_ptr<GameObject>>& allObjects)
	{
        auto collider = std::make_shared<BoxCollider>(Vector3(1, 2, 1));
        auto logGo = std::make_shared<GameObject>(nullptr, collider);
        //auto logMesh = std::make_shared<GameObject>(std::make_shared<MeshRenderer>(ShaderAssets::Lit(), MeshAssets::Log(), TextureAssets::Log()));
        logMesh->GetTransform()->SetParent(logGo->GetTransform().get());
        logMesh->GetRigidbody()->enabled = false;
        logMesh->GetTransform()->SetPositionY(-0.5f);

        allObjects.emplace_back(logGo);
        allObjects.emplace_back(logMesh);

        logGo->GetTransform()->SetLocalScale(Vector3::One * 5.0f);
        logGo->GetRigidbody()->gravityScale = 10;
        logGo->GetRigidbody()->mass = 1.0f;
        logGo->GetRigidbody()->InitCubeInertia();
        return logGo;
	}

    inline std::shared_ptr<GameObject> Gear(std::vector<std::shared_ptr<GameObject>>& allObjects)
    {
        auto collider = std::make_shared<BoxCollider>(Vector3(0.9f, 0.1f, 0.9f));
        auto gameObject = std::make_shared<GameObject>(std::make_shared<MeshRenderer>(ShaderAssets::Lit(), MeshAssets::Gear(), TextureAssets::Gear()), collider);

        allObjects.emplace_back(gameObject);

        gameObject->GetTransform()->SetLocalScale(Vector3::One * 0.1f);
        gameObject->GetRigidbody()->gravityScale = 10;
        gameObject->GetRigidbody()->mass = 5.0f;
        gameObject->GetRigidbody()->InitCubeInertia();

        return gameObject;
    }

    inline std::shared_ptr<GameObject> Rock(std::vector<std::shared_ptr<GameObject>>& allObjects)
    {
        auto collider = std::make_shared<BoxCollider>(Vector3(3.25f, 3.25f, 3.25f));
        auto gameObject = std::make_shared<GameObject>(std::make_shared<MeshRenderer>(ShaderAssets::Lit(), MeshAssets::Rock(), TextureAssets::Rock()), collider);

        allObjects.emplace_back(gameObject);

        gameObject->GetTransform()->SetLocalScale(Vector3::One * 0.05f);
        gameObject->GetRigidbody()->gravityScale = 10;
        gameObject->GetRigidbody()->mass = 100.0f;
        gameObject->GetRigidbody()->InitCubeInertia();
        return gameObject;
    }
}
*/