#include <memory>
#include <sstream>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneGame.h"
#include "Framework.h"

namespace PP
{
	namespace Class
	{
		// フレームレート計算
		void Framework::CalculateFrameStats()
		{
			// Code computes the average frames per second, and also the 
			// average time it takes to render one frame.  These stats 
			// are appended to the window caption bar.
			static int frames = 0;
			static float time_tlapsed = 0.0f;

			frames++;

			// Compute averages over one second period.
			if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
			{
				float fps = static_cast<float>(frames); // fps = frameCnt / 1
				float mspf = 1000.0f / fps;
				std::ostringstream outs;
				outs.precision(6);
				outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
				SetWindowTextA(hWnd, outs.str().c_str());

				// Reset for next average.
				frames = 0;
				time_tlapsed += 1.0f;
			}
			//elapsedTime = 1 == 0
			//	? timer.TimeInterval()
			//	: 1 / 60.0f;
		}

		// メッセージハンドラ
		LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (Graphics::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
				return true;

			switch (msg)
			{
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc;
				hdc = BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				break;
			}
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_CREATE:
				break;
			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case WM_ENTERSIZEMOVE:
				// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
				timer.Stop();
				break;
			case WM_EXITSIZEMOVE:
				// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
				// Here we reset everything based on the new window dimensions.
				timer.Start();
				break;
			default:
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
			return 0;
		}
	}

}