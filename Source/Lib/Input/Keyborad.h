#pragma once
#include <Windows.h>
// #include "Inc/KeyBoard.h"

namespace PP
{
	namespace Class
	{
		class LibKeyBoard
		{
		public:

			LibKeyBoard() {}
			~LibKeyBoard() {}

			void Update();

			int GetKeyInput(int key);

		private:

			BYTE keys[256];
			int pressCount[256];

		};
	}
}