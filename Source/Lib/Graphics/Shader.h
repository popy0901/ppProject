#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Lib/Graphics/RenderContext.h"
#include "Lib/Graphics/Model.h"

namespace PP
{
	namespace Class
	{
		class LibShader
		{
		public:
			LibShader() {}
			virtual ~LibShader() {}

			// �`��J�n
			virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;

			// �`��
			virtual void Draw(ID3D11DeviceContext* dc, const LibModel* model) = 0;

			// �`��I��
			virtual void End(ID3D11DeviceContext* context) = 0;
		};
	}

}