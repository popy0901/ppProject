#include "Lib/Input/Mouse.h"

/*static const int KeyMap[] =
{
	VK_LBUTTON,		// ���{�^��
	VK_MBUTTON,		// ���{�^��
	VK_RBUTTON,		// �E�{�^��
};

// �R���X�g���N�^
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;
}

// �X�V
void Mouse::Update()
{
	// �X�C�b�`���
	MouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
	{
		if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
		{
			newButtonState |= (1 << i);
		}
	}

	// �z�C�[��
	wheel[1] = wheel[0];
	wheel[3] = wheel[2];
	wheel[2] = 0;

	// �{�^�����X�V
	buttonState[1] = buttonState[0];	// �X�C�b�`����
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// �������u��
	buttonUp = ~newButtonState & buttonState[1];	// �������u��

	// �J�[�\���ʒu�̎擾
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// ��ʂ̃T�C�Y���擾����B
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;

	// ��ʕ␳
	positionX[1] = positionX[0];
	positionY[1] = positionY[1];
	positionX[0] = (LONG)(cursor.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
	positionY[0] = (LONG)(cursor.y / static_cast<float>(viewportH) * static_cast<float>(screenH));
}*/

namespace PP
{
	namespace Class
	{
		LibMouse::LibMouse(HWND hwnd)
			: hwnd(hwnd)
		{
			RECT rc;
			GetClientRect(hwnd, &rc);
			screenWidth = rc.right - rc.left;
			screenHeight = rc.bottom - rc.top;
		}

		void LibMouse::Update()
		{
			// �z�C�[��
			wheel[1] = wheel[0];
			wheel[3] = wheel[2];
			wheel[2] = 0;

			// �J�[�\���ʒu�̎擾
			POINT cursor;
			::GetCursorPos(&cursor);
			::ScreenToClient(hwnd, &cursor);

			// ��ʂ̃T�C�Y���擾����B
			RECT rc;
			GetClientRect(hwnd, &rc);
			UINT screenW = rc.right - rc.left;
			UINT screenH = rc.bottom - rc.top;
			UINT viewportW = screenWidth;
			UINT viewportH = screenHeight;

			// ��ʕ␳
			oldPosition.x = position.x;
			oldPosition.y = oldPosition.y;
			position.x = (LONG)(cursor.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
			position.y = (LONG)(cursor.y / static_cast<float>(viewportH) * static_cast<float>(screenH));

			// �ړ��ʎZ�o // TODO : �ۗ�
			//DirectX::XMFLOAT2 pos = {};
			//deltaPosition = {};
			//
			//deltaPosition = { position.x - oldPosition.x, position.y - oldPosition.y };
		}
	}
}