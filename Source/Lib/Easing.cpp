#include "Easing.h"
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include "Graphics/Graphics.h"
#include "Misc.h"

namespace PP
{
	namespace Class
	{
		float LibEasing::EaseInSine(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (1 - cosf((t * DirectX::XM_PI) / 2)));
		}

		float LibEasing::EaseOutSine(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (sinf((t * DirectX::XM_PI) / 2)));
		}

		float LibEasing::EaseInOutSine(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (-(cosf(DirectX::XM_PI * t) - 1) / 2));
		}

		float LibEasing::EaseInQuad(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (t * t));
		}

		float LibEasing::EaseOutQuad(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (1 - (1 - t) * (1 - t)));
		}

		float LibEasing::EaseInOutQuad(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float t22 = 2 * t * t;
			float T = -2 * t + 2;
			return static_cast<float>(a + c * (t < 0.5 ? t22 : 1 - pow(T, 2) / 2));
		}

		float LibEasing::EaseInCubic(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (t * t * t));
		}

		float LibEasing::EaseOutCubic(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = 1 - t;
			return static_cast<float>(a + c * (1 - pow(T, 3)));
		}

		float LibEasing::EaseInOutCubic(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float t43 = 4 * t * t * t;
			float T = -2 * t + 2;
			return static_cast<float>(a + c * (t < 0.5 ? t43 : 1 - pow(T, 3) / 2));
		}

		float LibEasing::EaseInQuart(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (t * t * t * t));
		}

		float LibEasing::EaseOutQuart(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = 1 - t;
			return static_cast<float>(a + c * (1 - pow(T, 4)));
		}

		float LibEasing::EaseInOutQuart(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float t84 = 8 * t * t * t * t;
			float T = -2 * t + 2;
			return static_cast<float>(a + c * (t < 0.5 ? t84 : 1 - pow(T, 4) / 2));
		}

		float LibEasing::EaseInQuint(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (t * t * t * t * t));
		}

		float LibEasing::EaseOutQuint(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = 1 - t;
			return static_cast<float>(a + c * (1 - pow(T, 5)));
		}

		float LibEasing::EaseInOutQuint(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float t165 = 16 * t * t * t * t * t;
			float T = -2 * t + 2;
			return static_cast<float>(a + c * (t < 0.5 ? t165 : 1 - pow(T, 5) / 2));
		}

		float LibEasing::EaseInExpo(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = 10 * t - 10;
			return static_cast<float>(a + c * (t == 0 ? 0 :
				pow(2, T)));
		}

		float LibEasing::EaseOutExpo(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = -10 * t;
			return static_cast<float>(a + c * (t == 1 ? 1 :
				1 - pow(2, T)));
		}

		float LibEasing::EaseInOutExpo(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = 20 * t - 10;
			float T2 = -20 * t + 10;
			return static_cast<float>(a + c * (t == 0 ? 0 :
				t == 1 ? 1 :
				t < 0.5 ? pow(2, T) / 2 :
				(2 - pow(2, T2)) / 2));
		}

		float LibEasing::EaseInCirc(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (1 - sqrt(1 - pow(t, 2))));
		}

		float LibEasing::EaseOutCirc(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float T = t - 1;
			return static_cast<float>(a + c * (sqrt(1 - pow(T, 2))));
		}

		float LibEasing::EaseInOutCirc(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			float t2 = 2 * t;
			float T = -2 * t + 2;
			return static_cast<float>(a + c * (t < 0.5 ? (1 - sqrt(1 - pow(t2, 2))) / 2 :
				(sqrt(1 - pow(T, 2)) + 1) / 2));
		}

		float LibEasing::EaseInBack(float a, float b, float timer, float totalTime)
		{
			const float c1 = 1.70158f;
			const float c3 = c1 + 1;

			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (c3 * t * t * t - c1 * t * t));
		}

		float LibEasing::EaseOutBack(float a, float b, float timer, float totalTime)
		{
			const float c1 = 1.70158f;
			const float c3 = c1 + 1;

			float c = b - a;
			float t = timer / totalTime;
			float T = t - 1;
			return static_cast<float>(a + c * (1 + c3 * pow(T, 3) + c1 * pow(T, 2)));
		}

		float LibEasing::EaseInOutBack(float a, float b, float timer, float totalTime)
		{
			const float c1 = 1.70158f;
			const float c2 = c1 * 1.525f;

			float c = b - a;
			float t = timer / totalTime;
			float T1 = 2 * t;
			float T2 = (c2 + 1) * 2 * t - c2;
			float T3 = 2 * t - 2;
			float T4 = (c2 + 1) * (t * 2 - 2) + c2;
			return static_cast<float>(a + c * (t < 0.5 ? (pow(T1, 2) * T2) / 2 :
				(pow(T3, 2) * (T4)+2) / 2));
		}

		float LibEasing::EaseInElastic(float a, float b, float timer, float totalTime)
		{
			const float c4 = (2 * DirectX::XM_PI) / 3;

			float c = b - a;
			float t = timer / totalTime;
			float T1 = 10 * t - 10;
			float T2 = (t * 10 - 10.75f) * c4;
			return static_cast<float>(a + c * (t == 0 ? 0 :
				t == 1 ? 1 :
				-pow(2, T1) * sin(T2)));
		}

		float LibEasing::EaseOutElastic(float a, float b, float timer, float totalTime)
		{
			const float c4 = (2 * DirectX::XM_PI) / 3;

			float c = b - a;
			float t = timer / totalTime;
			float T1 = -10 * t;
			float T2 = (t * 10 - 0.75f) * c4;
			return static_cast<float>(a + c * (t == 0 ? 0 :
				t == 1 ? 1 :
				pow(2, T1) * sin(T2) + 1));
		}

		float LibEasing::EaseInOutElastic(float a, float b, float timer, float totalTime)
		{
			const float c5 = (2 * DirectX::XM_PI) / 4.5f;

			float c = b - a;
			float t = timer / totalTime;
			float T1 = 20 * t - 10;
			float T2 = -20 * t + 10;
			float T3 = (20 * t - 11.125f) * c5;
			return static_cast<float>(a + c * (t == 0 ? 0 : t == 1 ? 1
				: t < 0.5 ? -(pow(2, T1) * sin(T3)) / 2
				: (pow(2, T2) * sin(T3)) / 2 + 1));
		}

		float LibEasing::Bounce(float t)
		{
			const float n1 = 7.5625;
			const float d1 = 2.75;

			if (t < 1 / d1) {
				return static_cast<float>((n1 * t * t));
			}
			else if (t < 2 / d1) {
				return static_cast<float>((n1 * (t -= 1.5f / d1) * t + 0.75f));
			}
			else if (t < 2.5f / d1) {
				return static_cast<float>((n1 * (t -= 2.25f / d1) * t + 0.9375f));
			}
			else {
				return static_cast<float>((n1 * (t -= 2.625f / d1) * t + 0.984375f));
			}
		}

		float LibEasing::EaseInBounce(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (1 - Bounce(1 - t)));
		}

		float LibEasing::EaseOutBounce(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (Bounce(t)));
		}

		float LibEasing::EaseInOutBounce(float a, float b, float timer, float totalTime)
		{
			float c = b - a;
			float t = timer / totalTime;
			return static_cast<float>(a + c * (t < 0.5
				? (1 - Bounce(1 - 2 * t)) / 2
				: (1 + Bounce(2 * t - 1)) / 2));
		}
	}
}