#pragma once

#include <memory>
#include <wrl.h>
#include "Lib/Graphics/Shader.h"

namespace PP
{
	namespace Class
	{
		class GaussShader /*: public LibShader*/
		{
		public:
			GaussShader(ID3D11Device* device);
			~GaussShader()/* override */{}

			void Render(ID3D11DeviceContext* dc, 
				float dx, float dy,
				float dw, float dh,
				float sx, float sy,
				float sw, float sh,
				float angle,
				float r, float g, float b, float a) const;

			// テクスチャ幅取得
			int GetTextureWidth() const { return textureWidth; }

			// テクスチャ高さ取得
			int GetTextureHeight() const { return textureHeight; }

			void SetTextureSize(int width, int height)
			{
				textureWidth = width;
				textureHeight = height;
			}

		private:

			struct Vertex
			{
				DirectX::XMFLOAT3	position;
				DirectX::XMFLOAT4	color;
				DirectX::XMFLOAT2	texcoord;
			};

			Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11Buffer>				buffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState = nullptr;

			int textureWidth = 0;
			int textureHeight = 0;
		};
	}
}