﻿#include "pch.h"
#include "FPSGame.h"

#include "Scene/Systems/Animation/WaypointDisplacementByTimeSystem.h"
#include "Scene/Systems/Animation/WaypointPositionUpdateSystem.h"
#include "Systems/EnemyMovementSystem.h"
#include "Systems/EnemySpawnSystem.h"
#include "Systems/FPSCameraRotationSystem.h"
#include "Systems/FPSMovementSystem.h"
#include "Systems/PlayerSpawnSystem.h"
#include "Systems/ProjectileLifeSystem.h"
#include "Systems/ProjectileMovementSystem.h"
#include "Systems/ShootSystem.h"

/*
 * There are some problems in this project
 * 1. (Done) We need Project system to have ability change assets of separate project
 * 2. (In progress) We need Prefabs system to avoid complete object creation from code
 * 3. (In progress) We need resource loading system (by name)
 * 4. We need our systems and components in editor, to setup entities visually
 * 5. We do not need to control engine pipeline in this project (OnUpdate, OnRender, etc)
 */
namespace Sandbox
{
	int score = 0;
	
	void FPSGame::OnAttach()
	{
		ResourceManager::Init("");
		AssetDatabase::ProcessAllAssets();

		SceneManager::LoadScene(AssetDatabase::GetAssetsPath() / "Scenes" / "TestScene.scene");

		auto& scene = SceneManager::GetActiveScene();

		scene->RegisterUpdateSystem<FPSMovementSystem>();
		scene->RegisterUpdateSystem<FPSCameraRotationSystem>();
		scene->RegisterUpdateSystem<ShootSystem>();
		scene->RegisterUpdateSystem<ProjectileMovementSystem>();
		scene->RegisterUpdateSystem<ProjectileLifeSystem>();
		scene->RegisterUpdateSystem<EnemySpawnSystem>();
		scene->RegisterUpdateSystem<PlayerSpawnSystem>();
		scene->RegisterUpdateSystem<EnemyMovementSystem>();
		
		scene->RegisterUpdateSystem<WaypointDisplacementByTimeSystem>();
		scene->RegisterUpdateSystem<WaypointPositionUpdateSystem>();
		
		scene->RegisterOneFrame<CollisionBeginEvent>();
		scene->RegisterOneFrame<CollisionEndEvent>();

		Application::Get().GetWindow().DisableCursor();
	}

	void FPSGame::OnDetach() { }

	void FPSGame::OnUpdate(const float deltaTime)
	{
		SceneManager::GetActiveScene()->OnUpdate(deltaTime);
		SceneManager::GetActiveScene()->OnPhysics(deltaTime);
		SceneManager::GetActiveScene()->OnLateUpdate(deltaTime);
		
		if (Input::IsKeyPressed(KeyCode::Escape))
			Application::Get().Close();
	}

	void FPSGame::OnRender()
	{
		SceneManager::GetActiveScene()->OnRender();
	}

	void FPSGame::OnGUIRender()
	{
		const auto time = Time::GetTimeSinceStartup().GetSeconds();
		const String timeStr = String("Time: ") + ToString(time) + String(" sec");
		GUI::DrawString(Vector2(0, 0), timeStr, 16.0f, Color::White);
		
		const String scoreStr = String("Score: ") + ToString(score);
		GUI::DrawString(Vector2(10, 32), scoreStr, 64.0f, Color::White);
		
		GUI::DrawCircle(GUI::GetWindowSize() / 2, 10, Color::White);
	}

	void FPSGame::OnEvent(Event& event) { }
}