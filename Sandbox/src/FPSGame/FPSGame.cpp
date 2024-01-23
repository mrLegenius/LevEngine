#include "pch.h"
#include "FPSGame.h"

namespace Sandbox
{
	using namespace LevEngine;
	
	void FPSGame::OnAttach()
	{
		App::Get().IsPlaying = true;

		AssetDatabase::ProcessAllAssets();
		ResourceManager::Init("");

		//TODO: Remove hard coded project name
		Project::Load("Sandbox.levproject");

		const auto startScene = Project::GetStartScene();
		if (startScene.empty() || !SceneManager::LoadScene(startScene))
			SceneManager::LoadScene(AssetDatabase::GetAssetsPath() / "Scenes" / "TestScene.scene");

		auto& scene = SceneManager::GetActiveScene();

		Audio::LoadBank(ToString(AssetDatabase::GetAssetsPath() / "Audio" / "Desktop" / "Master.bank"), true);
		Audio::LoadBank(ToString(AssetDatabase::GetAssetsPath() / "Audio" / "Desktop" / "Master.strings.bank"), true);

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
