#include "Lib/Graphics/LibLight.h"



namespace PP
{
	namespace Class
	{
		void LibLight::SetPosition(DirectX::XMFLOAT3 position)
		{
			this->position = position;
		}

		void LibLight::SetDirection(DirectX::XMFLOAT4 direction)
		{
			float d = sqrtf((direction.x * direction.x) + (direction.y * direction.y) + (direction.z * direction.z));

			if (d > 0)
			{
				direction.x /= d;
				direction.y /= d;
				direction.z /= d;
			}
			this->direction = direction;
		}

		void LibLight::SetColor(DirectX::XMFLOAT4 color)
		{
			this->color = color;
		}

		void LibLight::SetAmbient(DirectX::XMFLOAT4 ambient)
		{
			this->ambient = ambient;
		}
	}
}