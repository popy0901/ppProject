#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Lib/Graphics/Shader.h"
#include "Lib/Graphics/LibLight.h"
#include "Lib/Graphics/DebugRenderer.h"
#include "Lib/Graphics/LineRenderer.h"
#include "Lib/Graphics/ImGuiRenderer.h"
#include "Lib/Graphics/Primitive2D.h"

//#include "Graphics/GaussShader.h"

namespace PP
{
	namespace Class
	{
		// グラフィックス
		class Graphics
		{
		public:
			Graphics(HWND hWnd, bool isFullscreen);
			~Graphics();

			// インスタンス取得
			static Graphics& Instance() { return *instance; }

			// デバイス取得
			ID3D11Device* GetDevice() const { return device.Get(); }

			// デバイスコンテキスト取得
			ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

			// スワップチェーン取得
			IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

			// レンダーターゲットビュー取得
			ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

			// デプスステンシルビュー取得
			ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

			// ランバートシェーダ取得
			LibShader* GetLambert() const { return lambert.get(); }

			// 破壊シェーダ取得
			LibShader* GetDestraction() const { return destraction.get(); }
			void SetDestractionParamter(float scale, float angle, float position); // 試作

			// 影シェーダ取得
			LibShader* GetDepthShadow() const;
			void SetShadowTarget(const DirectX::XMFLOAT3& target);
			
			// 影描画シェーダ取得
			LibShader* GetRenderShadw() const { return renderShadow.get(); }

			// ガウスシェーダ取得
			//GaussShader* GetGauss() const { return gaussShader.get(); }

			// ライト取得
			LibLight* GetLight() const { return light.get(); }

			// スクリーン幅取得
			float GetScreenWidth() const { return screenWidth; }

			// スクリーン高さ取得
			float GetScreenHeight() const { return screenHeight; }

			// デバッグレンダラ取得
			DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

			// ラインレンダラ取得
			LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

			// ImGuiレンダラ取得
			ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }

			// 2Dプリミティブ取得
			Primitive2d* GetPrimitive2d() const { return primitive2d.get(); }

			void ScreenClear(const DirectX::XMFLOAT4& color = { 0, 0, 0, 0 });

		private:
			static Graphics* instance;

			Microsoft::WRL::ComPtr<ID3D11Device>			device = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext = nullptr;
			Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView = nullptr;

			std::unique_ptr<LibShader>						lambert = nullptr;
			std::unique_ptr<LibShader>						destraction = nullptr;
			std::unique_ptr<LibShader>						renderShadow = nullptr;
			//std::unique_ptr<GaussShader>					gaussShader = nullptr;

			std::unique_ptr<LibLight>						light = nullptr;

			std::unique_ptr<DebugRenderer>					debugRenderer = nullptr;
			std::unique_ptr<LineRenderer>					lineRenderer = nullptr;
			std::unique_ptr<ImGuiRenderer>					imguiRenderer = nullptr;
			std::unique_ptr<Primitive2d>					primitive2d = nullptr;

			float	screenWidth;
			float	screenHeight;
		};
	}
}