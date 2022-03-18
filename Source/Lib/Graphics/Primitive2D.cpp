#include "Lib/Graphics/Primitive2D.h"
#include "Lib/Graphics/Graphics.h"
#include "Lib/Misc.h"

namespace PP
{
	namespace Class
	{
		Primitive2d::Primitive2d(ID3D11Device* device)
		{
			HRESULT hr = S_OK;

			// 頂点バッファ生成
			{
				Vertex vertices[PRIMITIVE_VERTEX_NUM] = {};
				D3D11_BUFFER_DESC bufferDesc = {};
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.ByteWidth = sizeof(vertices);
				bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags = 0;
				bufferDesc.StructureByteStride = 0;
				D3D11_SUBRESOURCE_DATA subresourceData;
				subresourceData.pSysMem = vertices;
				subresourceData.SysMemPitch = 0;
				subresourceData.SysMemSlicePitch = 0;

				hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 頂点シェーダ
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\Primitive2dVS.cso", "rb");
				_ASSERT_EXPR_A(fp, "CSO File not found");

				// ファイルのサイズを求める
				fseek(fp, 0, SEEK_END);
				long csoSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				// メモリ上に頂点シェーダーデータを格納する領域を用意する
				std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
				fread(csoData.get(), csoSize, 1, fp);
				fclose(fp);

				// 頂点シェーダー生成
				hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// 入力レイアウト
				D3D11_INPUT_ELEMENT_DESC layout[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,     D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = device->CreateInputLayout(layout, ARRAYSIZE(layout), csoData.get(), csoSize, inputLayout.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ピクセルシェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\Primitive2dPS.cso", "rb");
				_ASSERT_EXPR_A(fp, "CSO File not found");

				// ファイルのサイズを求める
				fseek(fp, 0, SEEK_END);
				long csoSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				// メモリ上に頂点シェーダーデータを格納する領域を用意する
				std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
				fread(csoData.get(), csoSize, 1, fp);
				fclose(fp);

				// ピクセルシェーダー生成
				hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ブレンドステート
			{
				D3D11_BLEND_DESC blendDesc;
				::memset(&blendDesc, 0, sizeof(blendDesc));
				blendDesc.AlphaToCoverageEnable = false;
				blendDesc.IndependentBlendEnable = false;
				blendDesc.RenderTarget[0].BlendEnable = true;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

				HRESULT hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 深度ステンシルステート
			{
				D3D11_DEPTH_STENCIL_DESC dpthStencilDesc;
				::memset(&dpthStencilDesc, 0, sizeof(dpthStencilDesc));
				dpthStencilDesc.DepthEnable = true;
				dpthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				dpthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

				HRESULT hr = device->CreateDepthStencilState(&dpthStencilDesc, depthStencilState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ラスタライザーステート
			{
				D3D11_RASTERIZER_DESC resterizerDesc;
				::memset(&resterizerDesc, 0, sizeof(resterizerDesc));
				resterizerDesc.FrontCounterClockwise = false;
				resterizerDesc.DepthBias = 0;
				resterizerDesc.DepthBiasClamp = 0;
				resterizerDesc.SlopeScaledDepthBias = 0;
				resterizerDesc.DepthClipEnable = true;
				resterizerDesc.ScissorEnable = false;
				resterizerDesc.MultisampleEnable = true;
				resterizerDesc.FillMode = D3D11_FILL_SOLID;
				resterizerDesc.CullMode = D3D11_CULL_NONE;
				resterizerDesc.AntialiasedLineEnable = false;

				HRESULT hr = device->CreateRasterizerState(&resterizerDesc, rasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

		}

		void Primitive2d::Rect(ID3D11DeviceContext* dc,
			const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size,
			const DirectX::XMFLOAT2& center, float angle,
			const DirectX::XMFLOAT4& color)
		{
			if (size.x == 0.0f || size.y == 0.0f) return;

			D3D11_VIEWPORT viewport;
			UINT numViewports = 1;
			dc->RSGetViewports(&numViewports, &viewport);

			Vertex vertices[] = {
					{ { -0.0f, +1.0f, 0 }, color },
					{ { +1.0f, +1.0f, 0 }, color },
					{ { -0.0f, -0.0f, 0 }, color },
					{ { +1.0f, -0.0f, 0 }, color },
			};

			float sinAngle = sinf(angle);
			float cosAngle = cosf(angle);
			for (int i = 0; i < 4; i++)
			{
				vertices[i].position.x *= size.x;
				vertices[i].position.y *= size.y;
				vertices[i].position.x -= center.x;
				vertices[i].position.y -= center.y;

				float px = vertices[i].position.x;
				float py = vertices[i].position.y;
				vertices[i].position.x = px * cosAngle - py * sinAngle;
				vertices[i].position.y = px * sinAngle + py * cosAngle;
				vertices[i].position.x += pos.x;
				vertices[i].position.y += pos.y;
				vertices[i].position.x = -1.0f + vertices[i].position.x * 2 / viewport.Width;
				vertices[i].position.y = 1.0f - vertices[i].position.y * 2 / viewport.Height;
			}

			D3D11_MAPPED_SUBRESOURCE msr;
			dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			memcpy(msr.pData, vertices, sizeof(vertices));
			dc->Unmap(vertexBuffer.Get(), 0);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			dc->IASetInputLayout(inputLayout.Get());
			dc->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
			dc->RSSetState(rasterizerState.Get());
			dc->VSSetShader(vertexShader.Get(), nullptr, 0);
			dc->PSSetShader(pixelShader.Get(), nullptr, 0);
			dc->OMSetDepthStencilState(depthStencilState.Get(), 1);
			dc->Draw(4, 0);
		}

		void Primitive2d::Circle(ID3D11DeviceContext* dc,
			const DirectX::XMFLOAT2& pos, float radius,
			const DirectX::XMFLOAT4& color, int n)
		{
			if (n < 3 || radius <= 0.0f) return;
			if (n > 64) n = 64;

			D3D11_VIEWPORT viewport;
			UINT numViewports = 1;
			dc->RSGetViewports(&numViewports, &viewport);

			Vertex vertices[128] = {};
			float arc = DirectX::XM_PI * 2 / n;
			Vertex* p = &vertices[0];
			for (int i = 0; i <= n; i++)
			{
				p->position.x = pos.x + cosf(arc * i) * radius;
				p->position.y = pos.y + sinf(arc * i) * radius;
				p->position.x = -1.0f + p->position.x * 2 / viewport.Width;
				p->position.y = 1.0f - p->position.y * 2 / viewport.Height;

				p->color = color;

				p++;

				p->position.x = pos.x;
				p->position.y = pos.y;
				p->position.x = -1.0f + p->position.x * 2 / viewport.Width;
				p->position.y = 1.0f - p->position.y * 2 / viewport.Height;

				p->color = color;

				p++;
			}


			D3D11_MAPPED_SUBRESOURCE msr;
			dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
			memcpy(msr.pData, vertices, sizeof(vertices));
			dc->Unmap(vertexBuffer.Get(), 0);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			dc->IASetInputLayout(inputLayout.Get());
			dc->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
			dc->RSSetState(rasterizerState.Get());
			dc->VSSetShader(vertexShader.Get(), nullptr, 0);
			dc->PSSetShader(pixelShader.Get(), nullptr, 0);
			dc->OMSetDepthStencilState(depthStencilState.Get(), 1);
			dc->Draw((n + 1) * 2 - 1, 0);
		}

		void Primitive2d::Line(ID3D11DeviceContext* dc,
			const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2,
			const DirectX::XMFLOAT4& color, int width)
		{
			if (width <= 0.0f) return;

			DirectX::XMFLOAT2 V = { pos2.x - pos1.x, pos2.y - pos1.y };
			float v = sqrtf(V.x * V.x + V.y * V.y);
			float cpx = v * 0.5f;
			float cpy = width * 0.5f;
			float px = (pos1.x + pos2.x) * 0.5f;
			float py = (pos1.y + pos2.y) * 0.5f;
			float angle = atan2f(pos2.y - pos1.y, pos2.x - pos1.x);

			Rect(dc, { px, py }, { v, static_cast<float>(width) }, { cpx, cpy }, angle, color);
		}
	}
}