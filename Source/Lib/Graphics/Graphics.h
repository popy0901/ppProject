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
		// �O���t�B�b�N�X
		class Graphics
		{
		public:
			Graphics(HWND hWnd, bool isFullscreen);
			~Graphics();

			// �C���X�^���X�擾
			static Graphics& Instance() { return *instance; }

			// �f�o�C�X�擾
			ID3D11Device* GetDevice() const { return device.Get(); }

			// �f�o�C�X�R���e�L�X�g�擾
			ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

			// �X���b�v�`�F�[���擾
			IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

			// �����_�[�^�[�Q�b�g�r���[�擾
			ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

			// �f�v�X�X�e���V���r���[�擾
			ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

			// �����o�[�g�V�F�[�_�擾
			LibShader* GetLambert() const { return lambert.get(); }

			// �j��V�F�[�_�擾
			LibShader* GetDestraction() const { return destraction.get(); }
			void SetDestractionParamter(float scale, float angle, float position); // ����

			// �e�V�F�[�_�擾
			LibShader* GetDepthShadow() const;
			void SetShadowTarget(const DirectX::XMFLOAT3& target);
			
			// �e�`��V�F�[�_�擾
			LibShader* GetRenderShadw() const { return renderShadow.get(); }

			// �K�E�X�V�F�[�_�擾
			//GaussShader* GetGauss() const { return gaussShader.get(); }

			// ���C�g�擾
			LibLight* GetLight() const { return light.get(); }

			// �X�N���[�����擾
			float GetScreenWidth() const { return screenWidth; }

			// �X�N���[�������擾
			float GetScreenHeight() const { return screenHeight; }

			// �f�o�b�O�����_���擾
			DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

			// ���C�������_���擾
			LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

			// ImGui�����_���擾
			ImGuiRenderer* GetImGuiRenderer() const { return imguiRenderer.get(); }

			// 2D�v���~�e�B�u�擾
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