#pragma once

#include "Lib/Input/GamePad.h"
#include "Lib/Input/Mouse.h"
#include "Lib/Input/Keyborad.h"

namespace PP
{
	namespace Class
	{
		// �C���v�b�g
		class Input
		{
		public:
			Input(HWND hWnd);
			~Input() {}

		public:
			// �C���X�^���X�擾
			static Input& Instance() { return *instance; }

			// �X�V����
			void Update();

			// �Q�[���p�b�h�擾
			GamePad& GetGamePad() { return gamePad; }

			// �}�E�X�擾
			LibMouse& GetMouse() { return mouse; }

			// �L�[�{�[�h�擾
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