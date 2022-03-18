#include <thread>
#include "SceneLoad.h"
#include "SceneManager.h"
#include "Lib/Lib.h"

void SceneLoad::Init()
{
	PP::Image::Load("Data/Image/LoadIcon.png", sprite);

	// スレッド開始
	std::thread thread(LoadingThread, this);

	// スレッドの管理を破棄
	thread.detach();
}

void SceneLoad::Update(float elapsedTime)
{

	// 次シーンの準備が完了次第切り替え
	if (nextScene->IsFinish())
	{
		Sleep(300);
		SceneManager::Instance()->ChangeScene(nextScene);
	}
}

void SceneLoad::Render()
{
	static float angle = 0.0f;
	angle += 1.0f;
	PP::Image::Render({ 1600 - 128, 900 - 128 },
		{ 1.0f, 1.0f },
		angle, { 1.0f, 1.0f, 1.0f, 1.0f }, sprite);
}

void SceneLoad::DebugRender()
{

}

void SceneLoad::End()
{

}

void SceneLoad::LoadingThread(SceneLoad* scene)
{
	CoInitialize(nullptr);
	scene->nextScene->Init();
	CoUninitialize();
	scene->nextScene->SetFinish();
}