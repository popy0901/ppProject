#pragma once
#include <wrl.h>
#include <d3d11.h>

namespace PP
{
	namespace Class
	{
		class LibEasing
		{
		public:

			LibEasing() {}
			~LibEasing() {}

			float EaseInSine(float a, float b, float timer, float totalTime);

			float EaseOutSine(float a, float b, float timer, float totalTime);

			float EaseInOutSine(float a, float b, float timer, float totalTime);

			float EaseInQuad(float a, float b, float timer, float totalTime);

			float EaseOutQuad(float a, float b, float timer, float totalTime);

			float EaseInOutQuad(float a, float b, float timer, float totalTime);

			float EaseInCubic(float a, float b, float timer, float totalTime);

			float EaseOutCubic(float a, float b, float timer, float totalTime);

			float EaseInOutCubic(float a, float b, float timer, float totalTime);

			float EaseInQuart(float a, float b, float timer, float totalTime);

			float EaseOutQuart(float a, float b, float timer, float totalTime);

			float EaseInOutQuart(float a, float b, float timer, float totalTime);

			float EaseInQuint(float a, float b, float timer, float totalTime);

			float EaseOutQuint(float a, float b, float timer, float totalTime);

			float EaseInOutQuint(float a, float b, float timer, float totalTime);

			float EaseInExpo(float a, float b, float timer, float totalTime);

			float EaseOutExpo(float a, float b, float timer, float totalTime);

			float EaseInOutExpo(float a, float b, float timer, float totalTime);

			float EaseInCirc(float a, float b, float timer, float totalTime);

			float EaseOutCirc(float a, float b, float timer, float totalTime);

			float EaseInOutCirc(float a, float b, float timer, float totalTime);

			float EaseInBack(float a, float b, float timer, float totalTime);

			float EaseOutBack(float a, float b, float timer, float totalTime);

			float EaseInOutBack(float a, float b, float timer, float totalTime);

			float EaseInElastic(float a, float b, float timer, float totalTime);

			float EaseOutElastic(float a, float b, float timer, float totalTime);

			float EaseInOutElastic(float a, float b, float timer, float totalTime);

			float EaseInBounce(float a, float b, float timer, float totalTime);

			float EaseOutBounce(float a, float b, float timer, float totalTime);

			float EaseInOutBounce(float a, float b, float timer, float totalTime);

		private:

			float Bounce(float t);

		};
	}
}