#pragma once

#include <memory>
#include <wrl.h>
#include "Lib/Graphics/Shader.h"
#include "Lib/Graphics/GaussShader.h"

namespace PP
{
	namespace Class
	{
		class DepthShadowShader : public LibShader
		{
		public:
			DepthShadowShader(ID3D11Device* device);
			~DepthShadowShader() override {}

			bool CreateDepth(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS);
			bool CreateRenderTarget(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format);

			void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
			void Draw(ID3D11DeviceContext* dc, const LibModel* model) override;
			void End(ID3D11DeviceContext* dc) override;

			void SetShadowTarget(const DirectX::XMFLOAT3& target) { shadowTarget = target; }

			ID3D11DepthStencilView* GetDepthStencilView() { return DepthStencilView.Get(); }
			ID3D11RenderTargetView* GetRenderTargetView(int index) { return RenderTargetView[index].Get(); }
			ID3D11ShaderResourceView* GetShaderResourceView(int index) {return ShaderResourceView[index].Get(); }

		private:
			static	constexpr	u_int	KARNEL_SIZE = 16;
			static	constexpr	u_int	BUFFER_SIZE = KARNEL_SIZE * KARNEL_SIZE;

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

			struct ConstantBufferForShadow
			{
				DirectX::XMFLOAT4X4  LightViewProjection;
				DirectX::XMFLOAT4	 ShadowColor;
			};

			struct ConstantBufferForPerFrameGaussianBlur
			{
				DirectX::XMFLOAT4 Weight[BUFFER_SIZE];
				float KarnelSize;
				DirectX::XMFLOAT2 texcel;
				float dummy;
			};

			RenderContext& ShadowContext(); // 影用の行列に変換

			void CalcGaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma);

			Microsoft::WRL::ComPtr<ID3D11Buffer>			sceneConstantBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			meshConstantBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			subsetConstantBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			ConstantBufferGaussianBlur = nullptr;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState = nullptr;

			Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerState = nullptr;

			D3D11_TEXTURE2D_DESC texture2d_desc[2] = {};

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView[2] = {};
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView[2] = {};
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView = nullptr;

			std::shared_ptr<ConstantBufferForShadow>  shadowConstantBuffer = {};
			std::unique_ptr<GaussShader>			  gaussShader = nullptr;

			DirectX::XMFLOAT3 shadowTarget = { 0, 0, 0 };

		public:

			ConstantBufferForShadow* GetShadowConstantBuffer() { return shadowConstantBuffer.get(); }

		};



		class RenderShadowShader : public LibShader
		{
		public:
			RenderShadowShader(ID3D11Device* device);
			~RenderShadowShader() override {}

			void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
			void Draw(ID3D11DeviceContext* dc, const LibModel* model) override;
			void End(ID3D11DeviceContext* dc) override;

			DepthShadowShader* GetDepthShadow() const { return depthShadow.get(); }

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

				DirectX::XMFLOAT4	LightColor;		//ライトの色
				DirectX::XMFLOAT4	AmbientColor;	//環境光
				DirectX::XMFLOAT4	EyePos;			//カメラ

				DirectX::XMFLOAT4X4  LightViewProjection;
				DirectX::XMFLOAT4	 ShadowColor;
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

			std::shared_ptr<DepthShadowShader> depthShadow;
		};

	}
}