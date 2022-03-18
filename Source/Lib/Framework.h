#pragma once

#include <windows.h>
#include "HighResolutionTimer.h"
#include "Audio/Audio.h"
#include "Graphics/Graphics.h"
#include "Input/input.h"

namespace PP
{
	namespace Class
	{
		class Framework
		{
		public:
			Framework(HWND hWnd) : hWnd(hWnd) {}
			~Framework() {}

			LRESULT CALLBACK HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			void CalculateFrameStats();

			float GetElapsedTime() { return elapsedTime; }

			void SetElapsedTime(float elapsedTime) { this->elapsedTime = elapsedTime; }

		private:
			const HWND				hWnd;
			HighResolutionTimer		timer;

			float elapsedTime = 0.0f;
		};
	}
}
