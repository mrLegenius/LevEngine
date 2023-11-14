#pragma once
#include "..\..\..\..\LevEngine\src\Physics\PhysX\PhysicsBase.h"
namespace Sandbox
{
    class PhysicsPhysXTestSystem : public System
    {
    public:
        void Update(float deltaTime, entt::registry& registry) override
        {
            const auto physView = registry.view<PhysicsRigidbody>();
            if (!physView.empty()) return;
            
            //auto& scene = SceneManager::GetActiveScene();
            //const auto prefab = ResourceManager::LoadAsset<PrefabAsset>("CubePrefab");

            //const auto ground = scene->CreateEntity("Ground");
            //ground.GetComponent<Transform>().SetWorldRotation(Quaternion::CreateFromAxisAngle(Vector3(0.0f, 0.0f, 1.0f),PxPiDivTwo));
            //ground.GetComponent<Transform>().SetWorldPosition(Vector3(0.0f, 0.5f, 0.0f));
            //auto& rigidbody = ground.AddComponent<RigidbodyPhysX>();
            //rigidbody.SetActorPose(ground.GetComponent<Transform>());
            //rigidbody.SetActorType(RigidbodyType::Static);
            //rigidbody.AttachShapeGeometry(ColliderType::Capsule);
            //rigidbody.DetachShapeGeometry(0);
            //rigidbody.AttachShapeGeometry(ColliderType::Plane);
            
            //const auto cube0 = prefab->Instantiate(scene);
            //cube0.GetComponent<Transform>().SetWorldPosition(Vector3(0.0f, 15.0f, 0.0f));
            //cube0.GetComponent<Transform>().SetWorldRotation(Quaternion::CreateFromAxisAngle(Vector3(0.3f, 0.0f, 1.0f), 45));
            //auto& rigidbody0 = cube0.AddComponent<RigidbodyPhysX>();
            //rigidbody0.SetActorPose(cube0.GetComponent<Transform>());
            //rigidbody0.AttachShapeGeometry(ColliderType::Box);
            //rigidbody0.SetStaticFriction(0, 0, 0.2f);
            //rigidbody0.SetDynamicFriction(0, 0, 0.2f);
            //rigidbody0.SetRestitution(0, 0, 0.2f);

            //const auto cube1 = prefab->Instantiate(scene);
            //cube1.GetComponent<Transform>().SetWorldPosition(Vector3(0.0f, 30.0f, 0.0f));
            //cube1.GetComponent<Transform>().SetWorldRotation(Quaternion::CreateFromAxisAngle(Vector3(0.3f, 0.0f, 1.0f), 45));
            //auto& rigidbody1 = cube1.AddComponent<RigidbodyPhysX>();
            //rigidbody1.SetActorPose(cube1.GetComponent<Transform>());
            //rigidbody1.AttachShapeGeometry(ColliderType::Box);
            //rigidbody1.SetShapeGeometryParam(0, 1, 3);
        }
    };
}