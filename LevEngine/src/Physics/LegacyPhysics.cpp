#include "levpch.h"
#include "LegacyPhysics.h"

#include "Components\LegacyCollisionEvents.h"
#include "DataTypes/Map.h"
#include "DataTypes/Pair.h"
#include "DataTypes/Vector.h"
#include "Scene/Entity.h"
#include "Systems\LegacyForcesClearSystem.h"
#include "Systems\LegacyPositionUpdateSystem.h"
#include "Systems\LegacyVelocityUpdateSystem.h"

#include "PxPhysicsAPI.h"

namespace LevEngine
{
bool LegacyPhysics::AABBTest(
    const Vector3& posA, const Vector3& posB,
    const Vector3& halfSizeA, const Vector3& halfSizeB)
{
    const Vector3 delta = posB - posA;
    const Vector3 totalSize = halfSizeA + halfSizeB;

    return abs(delta.x) < totalSize.x
        && abs(delta.y) < totalSize.y
        && abs(delta.z) < totalSize.z;
}

bool LegacyPhysics::HasAABBIntersection(const LegacyBoxCollider& colliderA, const Transform& transformA,
	const LegacyBoxCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
{
	const Vector3 boxAPos = transformA.GetWorldPosition() + colliderA.offset;
	const Vector3 boxBPos = transformB.GetWorldPosition() + colliderB.offset;
	const Vector3 boxASize = colliderA.extents;
	const Vector3 boxBSize = colliderB.extents;

	const bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

	if (!overlap) return false;

	static constexpr Vector3 faces[6] =
	{
		Vector3(-1, 0, 0), Vector3(1, 0, 0),
		Vector3(0, -1, 0), Vector3(0, 1, 0),
		Vector3(0, 0, -1), Vector3(0, 0, 1),
	};

	const Vector3 maxA = boxAPos + boxASize;
	const Vector3 minA = boxAPos - boxASize;
	const Vector3 maxB = boxBPos + boxBSize;
	const Vector3 minB = boxBPos - boxBSize;
	const float distances[6] =
	{
		(maxB.x - minA.x), // distance of box 'b' to 'left' of 'a'.
		(maxA.x - minB.x), // distance of box 'b' to 'right' of 'a'.
		(maxB.y - minA.y), // distance of box 'b' to 'bottom' of 'a'.
		(maxA.y - minB.y), // distance of box 'b' to 'top' of 'a'.
		(maxB.z - minA.z), // distance of box 'b' to 'far' of 'a'.
		(maxA.z - minB.z), // distance of box 'b' to 'near' of 'a'.
	};
	float penetration = FLT_MAX;
	Vector3 bestAxis;

	for (int i = 0; i < 6; i++)
	{
		if (distances[i] < penetration)
		{
			penetration = distances[i];
			bestAxis = faces[i];
		}
	}
	collisionInfo.SetContactPoint(Vector3(), Vector3(), bestAxis, penetration);
	return true;
}

bool LegacyPhysics::HasSphereIntersection(const LegacySphereCollider& colliderA, const Transform& transformA,
	const LegacySphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
{
	const Vector3 posA = transformA.GetWorldPosition() + colliderA.offset;
	const Vector3 posB = transformB.GetWorldPosition() + colliderB.offset;

	const float radiiSum = colliderA.radius + colliderB.radius;
	Vector3 delta = posB - posA;

	const float deltaLength = delta.Length();

	if (deltaLength > radiiSum) return false;

	const float penetration = radiiSum - deltaLength;
	delta.Normalize();
	const Vector3 normal = delta;
	const Vector3 localA = normal * colliderA.radius;
	const Vector3 localB = -normal * colliderB.radius;

	collisionInfo.SetContactPoint(localA, localB, normal, penetration);
	return true;
}

bool LegacyPhysics::HasAABBSphereIntersection(const LegacyBoxCollider& colliderA, const Transform& transformA,
	const LegacySphereCollider& colliderB, const Transform& transformB, CollisionInfo& collisionInfo)
{
	const Vector3 posA = transformA.GetWorldPosition() + colliderA.offset;
	const Vector3 posB = transformB.GetWorldPosition() + colliderB.offset;
	const Vector3 delta = posB - posA;

	const Vector3 boxSize = colliderA.extents;

	const auto closestPointOnBox = Vector3(
		std::clamp(delta.x, -boxSize.x, boxSize.x),
		std::clamp(delta.y, -boxSize.y, boxSize.y),
		std::clamp(delta.z, -boxSize.z, boxSize.z));

	Vector3 localPoint = delta - closestPointOnBox;
	const float distance = localPoint.Length();

	if (distance > colliderB.radius) return false;

	localPoint.Normalize();

	const Vector3 collisionNormal = localPoint;
	const float penetration = colliderB.radius - distance;

	const Vector3 localA = Vector3();
	const Vector3 localB = -collisionNormal * colliderB.radius;

	collisionInfo.SetContactPoint(localA, localB, collisionNormal, penetration);
	return true;
}

void LegacyPhysics::HandleCollision(Transform& transformA, LegacyRigidbody& rigidbodyA, Transform& transformB, LegacyRigidbody& rigidbodyB, ContactPoint p)
{
	float totalMass = rigidbodyA.GetInverseMass() + rigidbodyB.GetInverseMass();

	// Separate them out using projection
	transformA.SetWorldPosition(transformA.GetWorldPosition()
		- p.normal
		* p.penetration
		* (rigidbodyA.GetInverseMass() / totalMass));

	transformB.SetWorldPosition(transformB.GetWorldPosition()
		+ p.normal
		* p.penetration
		* (rigidbodyB.GetInverseMass() / totalMass));

	// Impulse
	Vector3 relativeA = p.localA;
	Vector3 relativeB = p.localB;

	Vector3 angVelocityA = rigidbodyA.angularVelocity.Cross(relativeA);
	Vector3 angVelocityB = rigidbodyB.angularVelocity.Cross(relativeB);

	Vector3 fullVelocityA = rigidbodyA.velocity + angVelocityA;
	Vector3 fullVelocityB = rigidbodyB.velocity + angVelocityB;

	Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	float impulseForce = contactVelocity.Dot(p.normal);

	//Vector3 inertiaA = Vector3::Transform(relativeA.Cross(p.normal), rigidbodyA.GetInertiaTensor()).Cross(relativeA);
	Vector3 inertiaA = relativeA.Cross(p.normal).Cross(relativeA);
	//Vector3 inertiaB = Vector3::Transform(relativeB.Cross(p.normal), rigidbodyB.GetInertiaTensor()).Cross(relativeB);
	Vector3 inertiaB = relativeB.Cross(p.normal).Cross(relativeB);

	float angularEffect = (inertiaA + inertiaB).Dot(p.normal);

	float cRestitution = rigidbodyA.elasticity * rigidbodyB.elasticity; // disperse some kinetic energy

	float j = -(1.0f + cRestitution) * impulseForce / (totalMass + angularEffect);

	const Vector3 fullImpulse = p.normal * j;

	rigidbodyA.AddImpulse(-fullImpulse);
	rigidbodyB.AddImpulse(fullImpulse);

	rigidbodyA.AddAngularImpulse(relativeA.Cross(-fullImpulse));
	rigidbodyB.AddAngularImpulse(relativeB.Cross(fullImpulse));
}


void CollisionDetectionSystem(entt::registry& registry);
void UpdateCollisionList(entt::registry& registry);
void SphereCollisionSystem(entt::registry& registry);
void AABBSphereCollisionSystem(entt::registry& registry);
void AABBCollisionResolveSystem(entt::registry& registry);
static void RegisterCollision(entt::entity i, entt::entity j);

Entity ConvertEntity(const entt::entity entity, entt::registry& registry)
{
	return Entity(entt::handle(registry, entity));
}

static float dTOffset = 0;
using entity_pair = Pair<entt::entity, entt::entity>;
Map<entity_pair, int> collisions;

//static inline bool bTry = true;
void LegacyPhysics::Process(entt::registry& registry, float deltaTime)
{
	LEV_PROFILE_FUNCTION();

	/*
	if (bTry)
	{
		// declare variables
		physx::PxDefaultAllocator      mDefaultAllocatorCallback;
		physx::PxDefaultErrorCallback  mDefaultErrorCallback;
		physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
		physx::PxTolerancesScale       mToleranceScale;

		physx::PxFoundation* mFoundation = NULL;
		physx::PxPhysics* mPhysics = NULL;

		physx::PxScene* mScene = NULL;
		physx::PxMaterial* mMaterial = NULL;

		physx::PxPvd* mPvd = NULL;

		// init physx
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
		if (!mFoundation) throw("PxCreateFoundation failed!");
		mPvd = PxCreatePvd(*mFoundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
		//mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),true, mPvd);
		mToleranceScale.length = 100;        // typical length of an object
		mToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
		//mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

		physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = mDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		mScene = mPhysics->createScene(sceneDesc);

		physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		// create simulation
		mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 1, 0, 50), *mMaterial);
		mScene->addActor(*groundPlane);

		float halfExtent = .5f;
		physx::PxShape* shape = mPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mMaterial);
		physx::PxU32 size = 30;
		physx::PxTransform t(physx::PxVec3(0));
		for (physx::PxU32 i = 0; i < size; i++) {
			for (physx::PxU32 j = 0; j < size - i; j++) {
				physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
				physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(t.transform(localTm));
				body->attachShape(*shape);
				physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				mScene->addActor(*body);
			}
		}
		shape->release();

		// run simulation
		for (int i = 0; i < 200; i++)
		{
			mScene->simulate(1.0f / 60.0f);
			mScene->fetchResults(true);
		}
		
		bTry = false;
	}
	*/
	
	constexpr float iterationDt = 1.0f / 120.0f; //Ideally we'll have 120 physics updates a second 
	dTOffset += deltaTime; //We accumulate time delta here - there might be remainders from previous frame!

	const int iterationCount = static_cast<int>(dTOffset / iterationDt); //And split it up here

	const float subDt = deltaTime / static_cast<float>(iterationCount);	//How many seconds per iteration do we get?

	VelocityUpdateSystem(deltaTime, registry);

	for (int i = 0; i < iterationCount; ++i)
	{
		/*if (useBroadPhase) { //TODO: Add optimization
			BroadPhase();
			NarrowPhase();
		}
		else {*/
		CollisionDetectionSystem(registry);
		//}

		//This is our simple iterative solver - 
		//we just run things multiple times, slowly moving things forward
		//and then rechecking that the constraints have been met

		constexpr int constraintIterationCount = 10;
		const float constraintDt = subDt / static_cast<float>(constraintIterationCount);

		for (int j = 0; j < constraintIterationCount; ++j) {
			//UpdateConstraints(constraintDt);
			UpdatePositionSystem(constraintDt, registry);
		}

		dTOffset -= iterationDt;
	}
	ForcesClearSystem(deltaTime, registry);//Once we've finished with the forces, reset them to zero

	UpdateCollisionList(registry); //Remove any old collisions
}

void CollisionDetectionSystem(entt::registry& registry)
{
	LEV_PROFILE_FUNCTION();

	AABBCollisionResolveSystem(registry);
	SphereCollisionSystem(registry);
	AABBSphereCollisionSystem(registry);
}
void UpdateCollisionList(entt::registry& registry)
{
	LEV_PROFILE_FUNCTION();

	Vector<entity_pair> pairsToDelete;

	for (auto [pair, frames] : collisions)
	{
		auto a = pair.first;
		auto b = pair.second;
		if (frames == NumCollisionFrames)
		{
			if (const LegacyCollisionEvents* eventA = registry.try_get<LegacyCollisionEvents>(a))
			{
				if (eventA->onCollisionBegin)
					eventA->onCollisionBegin(ConvertEntity(a, registry), ConvertEntity(b, registry));
			}

			if (const LegacyCollisionEvents* eventB = registry.try_get<LegacyCollisionEvents>(b))
			{
				if (eventB->onCollisionBegin)
					eventB->onCollisionBegin(ConvertEntity(b, registry), ConvertEntity(a, registry));
			}
		}

		collisions[pair]--;

		if (frames < 0)
		{
			if (const LegacyCollisionEvents* eventA = registry.try_get<LegacyCollisionEvents>(a))
			{
				if (eventA->onCollisionEnd)
					eventA->onCollisionEnd(ConvertEntity(a, registry), ConvertEntity(b, registry));
			}


			if (const LegacyCollisionEvents* eventB = registry.try_get<LegacyCollisionEvents>(b))
			{
				if (eventB->onCollisionEnd)
					eventB->onCollisionEnd(ConvertEntity(b, registry), ConvertEntity(a, registry));
			}

			pairsToDelete.emplace_back(pair);
		}
	}

	for (auto pair : pairsToDelete)
		collisions.erase(pair);
}
void SphereCollisionSystem(entt::registry& registry)
{
	LEV_PROFILE_FUNCTION();

	const auto view = registry.group<>(entt::get<Transform, LegacyRigidbody, LegacySphereCollider>);
	const auto first = view.begin();
	const auto last = view.end();

	for (auto i = first; i != last; ++i)
	{
		auto [transform1, rigidbody1, colliderA] = view.get<Transform, LegacyRigidbody, LegacySphereCollider>(*i);
		for (auto j = i + 1; j != last; ++j)
		{
			auto [transform2, rigidbody2, colliderB] = view.get<Transform, LegacyRigidbody, LegacySphereCollider>(*j);

			if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
			CollisionInfo info;

			if (LegacyPhysics::HasSphereIntersection(
				colliderA,
				transform1,
				colliderB,
				transform2,
				info))
			{
				LegacyPhysics::HandleCollision(transform1, rigidbody1, transform2, rigidbody2, info.point);
				RegisterCollision(*i, *j);
			}
		}
	}
}
void AABBSphereCollisionSystem(entt::registry& registry)
{
	LEV_PROFILE_FUNCTION();

	const auto view = registry.group<>(entt::get<Transform, LegacyRigidbody, LegacyBoxCollider>);
	const auto first = view.begin();
	const auto last = view.end();

	const auto viewB = registry.group<>(entt::get<Transform, LegacyRigidbody, LegacySphereCollider>);
	const auto firstB = viewB.begin();
	const auto lastB = viewB.end();

	for (auto i = first; i != last; ++i)
	{
		auto [transform1, rigidbody1, colliderA] = view.get<Transform, LegacyRigidbody, LegacyBoxCollider>(*i);
		for (auto j = firstB; j != lastB; ++j)
		{
			if (*i == *j) continue;

			auto [transform2, rigidbody2, colliderB] = viewB.get<Transform, LegacyRigidbody, LegacySphereCollider>(*j);

			if (!rigidbody1.enabled || !rigidbody2.enabled) continue;
			CollisionInfo info;

			if (LegacyPhysics::HasAABBSphereIntersection(
				colliderA,
				transform1,
				colliderB,
				transform2,
				info))
			{
				LegacyPhysics::HandleCollision(transform1, rigidbody1, transform2, rigidbody2, info.point);
				RegisterCollision(*i, *j);
			}
		}
	}
}
void AABBCollisionResolveSystem(entt::registry& registry)
{
	LEV_PROFILE_FUNCTION();

	const auto group = registry.group<Transform>(entt::get<LegacyRigidbody, LegacyBoxCollider>);
	const auto first = group.begin();
	const auto last = group.end();

	for (auto i = first; i != last; ++i)
	{
		auto [transform1, rigidbody1, boxCollider1] = group.get<Transform, LegacyRigidbody, LegacyBoxCollider>(*i);
		for (auto j = i + 1; j != last; ++j)
		{
			auto [transform2, rigidbody2, boxCollider2] = group.get<Transform, LegacyRigidbody, LegacyBoxCollider>(*j);

			if (!rigidbody1.enabled || !rigidbody2.enabled) continue;

			CollisionInfo info;

			if (LegacyPhysics::HasAABBIntersection(
				boxCollider1,
				transform1,
				boxCollider2,
				transform2,
				info))
			{
				LegacyPhysics::HandleCollision(transform1, rigidbody1, transform2, rigidbody2, info.point);
				RegisterCollision(*i, *j);
			}
		}
	}
}

static void RegisterCollision(entt::entity i, entt::entity j)
{
	auto exist = collisions.find(entity_pair(i, j));
	if (exist == collisions.end())
		exist = collisions.find(entity_pair(j, i));

	if (exist == collisions.end())
		collisions.emplace(entity_pair(i, j), NumCollisionFrames);
	else
		exist->second = NumCollisionFrames - 1;
}
}
