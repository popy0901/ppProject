#include "Lib/Lib.h"
#include "SceneManager.h"
#include "SceneTitle.h"

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	PP::Init(L"Lib", SCREEN_WIDTH, SCREEN_HEIGHT);

	// ループ前初期化処理
	{
		SceneManager::Instance()->ChangeScene(new SceneTitle);
	}

	while (PP::Loop())
	{
		PP::Screen::Clear({ 0.0f, 0.0f, 0.0f, 1 });

		// ループ中更新処理
		{
			SceneManager::Instance()->Update(PP::System::GetElapsedTime());

			SceneManager::Instance()->Render();
			SceneManager::Instance()->DebugRender();
		}

		PP::Screen::Flip();
	}

	// ループ後終了処理
	{
		SceneManager::Instance()->End();
	}

	PP::End();


}
