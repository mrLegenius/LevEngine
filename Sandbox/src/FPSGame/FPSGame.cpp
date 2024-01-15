#include "pch.h"
#include "FPSGame.h"

#include "Project.h"
#include "Assets/PrefabAsset.h"
#include "Events/ApplicationEvent.h"
#include "Scripting/ScriptingManager.h"
#include "Systems/CollisionHandleSystem.h"
#include "Systems/EnemyMovementSystem.h"
#include "Systems/EnemySpawnSystem.h"
#include "Systems/FPSCameraRotationSystem.h"
#include "Systems/FPSMovementSystem.h"
#include "Systems/PlayerSpawnSystem.h"
#include "Systems/ProjectileLifeSystem.h"
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
	using namespace LevEngine;
	int score = 0;
	
	void FPSGame::OnAttach()
	{
		ResourceManager::Init("");
		AssetDatabase::ProcessAllAssets();

		//TODO: Remove hard coded project name
		Project::Load("Sandbox.levproject");

		const auto startScene = Project::GetStartScene();
		if (startScene.empty() || !SceneManager::LoadScene(startScene))
			SceneManager::LoadScene(AssetDatabase::GetAssetsPath() / "Scenes" / "TestScene.scene");

		auto& scene = SceneManager::GetActiveScene();

		Audio::LoadBank(ToString(AssetDatabase::GetAssetsPath() / "Audio" / "Desktop" / "Master.bank"), true);
		Audio::LoadBank(ToString(AssetDatabase::GetAssetsPath() / "Audio" / "Desktop" / "Master.strings.bank"), true);

		/*scene->RegisterUpdateSystem<PlayerSpawnSystem>();
		scene->RegisterUpdateSystem<FPSMovementSystem>();
		scene->RegisterUpdateSystem<FPSCameraRotationSystem>();
		scene->RegisterUpdateSystem<ShootSystem>();
		scene->RegisterUpdateSystem<CollisionHandleSystem>();
		scene->RegisterUpdateSystem<ProjectileLifeSystem>();
		scene->RegisterUpdateSystem<EnemySpawnSystem>();
		scene->RegisterUpdateSystem<EnemyMovementSystem>();*/

		scene->OnInit();
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
		SceneManager::GetActiveScene()->OnGUIRender();
	}

	void FPSGame::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<WindowFocusEvent>(BIND_EVENT_FN(FPSGame::OnWindowFocus));
	}

	bool FPSGame::OnWindowFocus(WindowFocusEvent& e)
	{
		App::Get().GetWindow().DisableCursor();
		return false;
	}
}
