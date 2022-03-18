#include "Lib/Input/Mouse.h"

/*static const int KeyMap[] =
{
	VK_LBUTTON,		// 左ボタン
	VK_MBUTTON,		// 中ボタン
	VK_RBUTTON,		// 右ボタン
};

// コンストラクタ
Mouse::Mouse(HWND hWnd)
	: hWnd(hWnd)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	screenWidth = rc.right - rc.left;
	screenHeight = rc.bottom - rc.top;
}

// 更新
void Mouse::Update()
{
	// スイッチ情報
	MouseButton newButtonState = 0;

	for (int i = 0; i < ARRAYSIZE(KeyMap); ++i)
	{
		if (::GetAsyncKeyState(KeyMap[i]) & 0x8000)
		{
			newButtonState |= (1 << i);
		}
	}

	// ホイール
	wheel[1] = wheel[0];
	wheel[3] = wheel[2];
	wheel[2] = 0;

	// ボタン情報更新
	buttonState[1] = buttonState[0];	// スイッチ履歴
	buttonState[0] = newButtonState;

	buttonDown = ~buttonState[1] & newButtonState;	// 押した瞬間
	buttonUp = ~newButtonState & buttonState[1];	// 離した瞬間

	// カーソル位置の取得
	POINT cursor;
	::GetCursorPos(&cursor);
	::ScreenToClient(hWnd, &cursor);

	// 画面のサイズを取得する。
	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT screenW = rc.right - rc.left;
	UINT screenH = rc.bottom - rc.top;
	UINT viewportW = screenWidth;
	UINT viewportH = screenHeight;

	// 画面補正
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
			// ホイール
			wheel[1] = wheel[0];
			wheel[3] = wheel[2];
			wheel[2] = 0;

			// カーソル位置の取得
			POINT cursor;
			::GetCursorPos(&cursor);
			::ScreenToClient(hwnd, &cursor);

			// 画面のサイズを取得する。
			RECT rc;
			GetClientRect(hwnd, &rc);
			UINT screenW = rc.right - rc.left;
			UINT screenH = rc.bottom - rc.top;
			UINT viewportW = screenWidth;
			UINT viewportH = screenHeight;

			// 画面補正
			oldPosition.x = position.x;
			oldPosition.y = oldPosition.y;
			position.x = (LONG)(cursor.x / static_cast<float>(viewportW) * static_cast<float>(screenW));
			position.y = (LONG)(cursor.y / static_cast<float>(viewportH) * static_cast<float>(screenH));

			// 移動量算出 // TODO : 保留
			//DirectX::XMFLOAT2 pos = {};
			//deltaPosition = {};
			//
			//deltaPosition = { position.x - oldPosition.x, position.y - oldPosition.y };
		}
	}
}