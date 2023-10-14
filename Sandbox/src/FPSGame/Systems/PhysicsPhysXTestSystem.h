#pragma once
#include "Physics/PhysX/PhysicsPhysX.h"
namespace Sandbox
{
    class PhysicsPhysXTestSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto physView = registry.view<RigidbodyPhysX>();
            if (!physView.empty()) return;
            
            auto& scene = SceneManager::GetActiveScene();
            const auto prefab = ResourceManager::LoadAsset<PrefabAsset>("CubePrefab");

            const auto ground = scene->CreateEntity("Ground");
            ground.GetComponent<Transform>().SetWorldRotation(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 0.0f, 1.0f),PxPiDivTwo));
            ground.GetComponent<Transform>().SetWorldPosition(Vector3(0.0f, 0.5f, 0.0f));
            auto& rigidbody = ground.AddComponent<RigidbodyPhysX>(ground.GetComponent<Transform>());
            rigidbody.SetActorType(PxActorType::eRIGID_STATIC);
            rigidbody.AttachShapeGeometry(PxGeometryType::eCAPSULE);
            rigidbody.DetachShapeGeometryType(0);
            rigidbody.AttachShapeGeometry(PxGeometryType::ePLANE);
            
            const auto cube0 = prefab->Instantiate(scene);
            cube0.GetComponent<Transform>().SetWorldPosition(Vector3(0.0f, 15.0f, 0.0f));
            cube0.GetComponent<Transform>().SetWorldRotation(Quaternion::CreateFromAxisAngle(Vector3(0.3f, 0.0f, 1.0f), 45));
            auto& rigidbody0 = cube0.AddComponent<RigidbodyPhysX>(cube0.GetComponent<Transform>());
            rigidbody0.AttachShapeGeometry(PxGeometryType::eBOX);
            rigidbody0.SetStaticFriction(0, 0, 0.2f);
            rigidbody0.SetDynamicFriction(0, 0, 0.2f);
            rigidbody0.SetRestitution(0, 0, 0.2f);

            const auto cube1 = prefab->Instantiate(scene);
            cube1.GetComponent<Transform>().SetWorldScale(Vector3(2.0f, 6.0f, 4.0f));
            cube1.GetComponent<Transform>().SetWorldPosition(Vector3(0.0f, 30.0f, 0.0f));
            cube1.GetComponent<Transform>().SetWorldRotation(Quaternion::CreateFromAxisAngle(Vector3(0.3f, 0.0f, 1.0f), 45));
            auto& rigidbody1 = cube1.AddComponent<RigidbodyPhysX>(cube1.GetComponent<Transform>());
            rigidbody1.AttachShapeGeometry(PxGeometryType::eBOX);
            rigidbody1.SetShapeGeometryParams(0, 1.0f, 3.0f, 2.0f);
            rigidbody1.SetActorMass(10);
        }
    };
}