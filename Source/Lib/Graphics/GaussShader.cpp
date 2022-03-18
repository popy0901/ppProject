//#include <stdio.h> 
//#include <WICTextureLoader.h>
#include "Lib/Graphics/GaussShader.h"
#include "Lib/Misc.h"
#include "Lib/Graphics/Graphics.h"

namespace PP
{
	namespace Class
	{
		GaussShader::GaussShader(ID3D11Device* device)
		{
			HRESULT hr = S_OK;

			Vertex vertices[] = {
				{ DirectX::XMFLOAT3(-0.5, +0.5, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0,0) },
				{ DirectX::XMFLOAT3(+0.5, +0.5, 0), DirectX::XMFLOAT4(1, 0, 0, 1), DirectX::XMFLOAT2(1,0) },
				{ DirectX::XMFLOAT3(-0.5, -0.5, 0), DirectX::XMFLOAT4(0, 1, 0, 1), DirectX::XMFLOAT2(0,1) },
				{ DirectX::XMFLOAT3(+0.5, -0.5, 0), DirectX::XMFLOAT4(0, 0, 1, 1), DirectX::XMFLOAT2(1,1) },
			};

			// ���_�o�b�t�@�̐���
			{
				// ���_�o�b�t�@���쐬���邽�߂̐ݒ�I�v�V����
				D3D11_BUFFER_DESC buffer_desc = {};
				::memset(&buffer_desc, 0, sizeof(buffer_desc));
				buffer_desc.ByteWidth = sizeof(vertices);	// �o�b�t�@�i�f�[�^���i�[������ꕨ�j�̃T�C�Y
				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// ���_�o�b�t�@�Ƃ��ăo�b�t�@���쐬����B
				buffer_desc.CPUAccessFlags = 0;	
				buffer_desc.MiscFlags = 0;
				buffer_desc.StructureByteStride = 0;
				// ���_�o�b�t�@�ɒ��_�f�[�^�����邽�߂̐ݒ�
				D3D11_SUBRESOURCE_DATA subresource_data = {};
				::memset(&subresource_data, 0, sizeof(subresource_data));
				subresource_data.pSysMem = vertices;	// �����Ɋi�[���������_�f�[�^�̃A�h���X��n�����Ƃ�CreateBuffer()���Ƀf�[�^�����邱�Ƃ��ł���B
				subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
				subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
				// ���_�o�b�t�@�I�u�W�F�N�g�̐���
				hr = device->CreateBuffer(&buffer_desc, &subresource_data, &buffer);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ���_�V�F�[�_�[
			{
				// �t�@�C�����J��
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\GaussVS.cso", "rb");
				_ASSERT_EXPR_A(fp, "CSO File not found");

				// �t�@�C���̃T�C�Y�����߂�
				fseek(fp, 0, SEEK_END);
				long csoSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
				std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
				fread(csoData.get(), csoSize, 1, fp);
				fclose(fp);

				// ���_�V�F�[�_�[����
				HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// ���̓��C�A�E�g
				D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// �s�N�Z���V�F�[�_�[
			{
				// �t�@�C�����J��
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\GaussPS.cso", "rb");
				_ASSERT_EXPR_A(fp, "CSO File not found");

				// �t�@�C���̃T�C�Y�����߂�
				fseek(fp, 0, SEEK_END);
				long csoSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
				std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
				fread(csoData.get(), csoSize, 1, fp);
				fclose(fp);

				// �s�N�Z���V�F�[�_�[����
				hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// �u�����h�X�e�[�g
			{
				D3D11_BLEND_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.AlphaToCoverageEnable = false;
				desc.IndependentBlendEnable = false;
				desc.RenderTarget[0].BlendEnable = true;
				desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// �[�x�X�e���V���X�e�[�g
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.DepthEnable = true;
				desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

				HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ���X�^���C�U�[�X�e�[�g
			{
				D3D11_RASTERIZER_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.FrontCounterClockwise = false;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0;
				desc.SlopeScaledDepthBias = 0;
				desc.DepthClipEnable = true;
				desc.ScissorEnable = false;
				desc.MultisampleEnable = true;
				desc.FillMode = D3D11_FILL_SOLID;
				desc.CullMode = D3D11_CULL_NONE;
				desc.AntialiasedLineEnable = false;

				HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}
		}

		void GaussShader::Render(ID3D11DeviceContext* dc,
			float dx, float dy,
			float dw, float dh,
			float sx, float sy,
			float sw, float sh,
			float angle,
			float r, float g, float b, float a) const
		{
			D3D11_VIEWPORT viewport;
			UINT numViewports = 1;
			
			dc->RSGetViewports(&numViewports, &viewport);
			float screenWidth = viewport.Width;
			float screenHeight = viewport.Height;

			screenWidth = Graphics::Instance().GetScreenWidth();
			screenHeight = Graphics::Instance().GetScreenHeight();

			dc->IASetInputLayout(inputLayout.Get());
			dc->VSSetShader(vertexShader.Get(), NULL, NULL);
			dc->PSSetShader(pixelShader.Get(), NULL, NULL);

			Vertex data[4];

			data[0].position.x = dx;
			data[0].position.y = dy;
			data[0].position.z = 0.0f;

			data[1].position.x = dx + dw;
			data[1].position.y = dy;
			data[1].position.z = 0.0f;

			data[2].position.x = dx;
			data[2].position.y = dy + dh;
			data[2].position.z = 0.0f;

			data[3].position.x = dx + dw;
			data[3].position.y = dy + dh;
			data[3].position.z = 0.0f;

			//��]�̒��S
			float workPosX = dx + dw * 0.5f;
			float workPosY = dy + dh * 0.5f;

			//��]����
			for (int i = 0; i < 4; i++) {

				float workX = data[i].position.x - workPosX;
				float workY = data[i].position.y - workPosY;
				data[i].position.x = workX * cosf(angle) - workY * sinf(angle) + workPosX;
				data[i].position.y = workX * sinf(angle) + workY * cosf(angle) + workPosY;
				data[i].position.z = 0.0f;
			}

			// ���K���f�o�C�X���W�n
			for (int i = 0; i < 4; i++) {
				data[i].position.x = 2.0f * data[i].position.x / screenWidth - 1.0f;
				data[i].position.y = 1.0f - 2.0f * data[i].position.y / screenHeight;
				data[i].position.z = 0.0f;
			}

			//�e�N�X�`�����W�ݒ�
			data[0].texcoord.x = sx;
			data[0].texcoord.y = sy;
			data[1].texcoord.x = sx + sw;
			data[1].texcoord.y = sy;
			data[2].texcoord.x = sx;
			data[2].texcoord.y = sy + sh;
			data[3].texcoord.x = sx + sw;
			data[3].texcoord.y = sy + sh;

			//UV���W
			for (int i = 0; i < 4; i++) {
				data[i].texcoord.x = data[i].texcoord.x / textureWidth; // dss->GetWidth(1);
				data[i].texcoord.y = data[i].texcoord.y / textureHeight; // dss->GetHeight(1);
			}

			//���_�J���[
			data[0].color = { r, g, b, a };
			data[1].color = { r, g, b, a };
			data[2].color = { r, g, b, a };
			data[3].color = { r, g, b, a };

			//�@��
			//data[0].Normal = XMFLOAT3(0, 0, 1);
			//data[1].Normal = XMFLOAT3(0, 0, 1);
			//data[2].Normal = XMFLOAT3(0, 0, 1);
			//data[3].Normal = XMFLOAT3(0, 0, 1);

			//���_�f�[�^�X�V
			dc->UpdateSubresource(buffer.Get(), 0, NULL, data, 0, 0);

			//	���_�o�b�t�@�̎w��
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(
				0, 1, buffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
				&stride,		// �P���_�̃T�C�Y
				&offset			// �J�n�ʒu
			);
			dc->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
			);

			dc->OMSetDepthStencilState(depthStencilState.Get(), 1);

			dc->Draw(4, 0);

			//�V�F�[�_������
			dc->VSSetShader(NULL, NULL, 0);
			dc->PSSetShader(NULL, NULL, 0);

		}
	}
}