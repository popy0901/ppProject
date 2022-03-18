#include "Lib/Input/Keyborad.h"


namespace PP
{
	namespace Class
	{
		void LibKeyBoard::Update()
		{
			GetKeyboardState(keys);
			for (int i = 0; i < 256; i++)
			{
				int old = keys[i];

				if (keys[i] & 0x80)
				{
					pressCount[i]++;
				}
				else
				{
					pressCount[i] = 0;
				}
			}
		}

		int LibKeyBoard::GetKeyInput(int key)
		{
			return pressCount[key];
		}
	}
}