#pragma once

#include "Lib/Input/GamePad.h"
#include "Lib/Input/Mouse.h"
#include "Lib/Input/Keyborad.h"

namespace PP
{
	namespace Class
	{
		// インプット
		class Input
		{
		public:
			Input(HWND hWnd);
			~Input() {}

		public:
			// インスタンス取得
			static Input& Instance() { return *instance; }

			// 更新処理
			void Update();

			// ゲームパッド取得
			GamePad& GetGamePad() { return gamePad; }

			// マウス取得
			LibMouse& GetMouse() { return mouse; }

			// キーボード取得
			LibKeyBoard& GetKeyBoard() { return keyBoard; }

		private:
			static Input* instance;
			GamePad				gamePad;
			//Mouse				mouse;
			LibKeyBoard			keyBoard;
			LibMouse			mouse;
		};

	}
}