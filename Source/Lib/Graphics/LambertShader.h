#pragma once

#include <memory>
#include <wrl.h>
#include "Lib/Graphics/Shader.h"

namespace PP
{
	namespace Class
	{
		class LambertShader : public LibShader
		{
		public:
			LambertShader(ID3D11Device* device);
			~LambertShader() override {}

			void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
			void Draw(ID3D11DeviceContext* dc, const LibModel* model) override;
			void End(ID3D11DeviceContext* dc) override;

		private:
			static const int MaxBones = 128;

			struct CbScene
			{
				DirectX::XMFLOAT4X4	viewProjection;
				DirectX::XMFLOAT4	lightDirection;
			};

			struct CbMesh
			{
				DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
			};

			struct CbSubset
			{
				DirectX::XMFLOAT4	materialColor;
			};


			Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState = nullptr;

			Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState = nullptr;
		};
	}
}