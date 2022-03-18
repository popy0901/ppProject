#include "SceneManager.h"



void SceneManager::Update(float elapsedTime)
{
	if (currentScene != nullptr)
	{
		currentScene->Update(elapsedTime);
	}
}

void SceneManager::Render()
{
	if (currentScene != nullptr)
	{
		currentScene->Render();
	}
}

void SceneManager::DebugRender()
{
	if (currentScene != nullptr)
	{
		currentScene->DebugRender();
	}
}

void SceneManager::End()
{
	if (currentScene != nullptr)
	{
		currentScene->End();
	}
}

void SceneManager::ChangeScene(Scene* scene)
{
	if (currentScene != nullptr) End();
	SetChangeType(NONE);
	currentScene.reset(scene);

	if (!currentScene->IsFinish())
	{
		currentScene->Init();
		currentScene->SetFinish();
	}
}