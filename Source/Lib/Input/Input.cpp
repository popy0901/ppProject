#include "Lib/Input/Input.h"



namespace PP
{
	namespace Class
	{
		Input* Input::instance = nullptr;

		// コンストラクタ
		Input::Input(HWND hWnd)
			: mouse(hWnd)
		{
			instance = this;
		}

		// 更新処理
		void Input::Update()
		{
			gamePad.Update();
			mouse.Update();
			keyBoard.Update();
		}

	}
}