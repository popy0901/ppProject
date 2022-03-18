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

			// 頂点バッファの生成
			{
				// 頂点バッファを作成するための設定オプション
				D3D11_BUFFER_DESC buffer_desc = {};
				::memset(&buffer_desc, 0, sizeof(buffer_desc));
				buffer_desc.ByteWidth = sizeof(vertices);	// バッファ（データを格納する入れ物）のサイズ
				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとしてバッファを作成する。
				buffer_desc.CPUAccessFlags = 0;	
				buffer_desc.MiscFlags = 0;
				buffer_desc.StructureByteStride = 0;
				// 頂点バッファに頂点データを入れるための設定
				D3D11_SUBRESOURCE_DATA subresource_data = {};
				::memset(&subresource_data, 0, sizeof(subresource_data));
				subresource_data.pSysMem = vertices;	// ここに格納したい頂点データのアドレスを渡すことでCreateBuffer()時にデータを入れることができる。
				subresource_data.SysMemPitch = 0; //Not use for vertex buffers.
				subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
				// 頂点バッファオブジェクトの生成
				hr = device->CreateBuffer(&buffer_desc, &subresource_data, &buffer);
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 頂点シェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\GaussVS.cso", "rb");
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
				HRESULT hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// 入力レイアウト
				D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ピクセルシェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\GaussPS.cso", "rb");
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

			// 深度ステンシルステート
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.DepthEnable = true;
				desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

				HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ラスタライザーステート
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

			//回転の中心
			float workPosX = dx + dw * 0.5f;
			float workPosY = dy + dh * 0.5f;

			//回転処理
			for (int i = 0; i < 4; i++) {

				float workX = data[i].position.x - workPosX;
				float workY = data[i].position.y - workPosY;
				data[i].position.x = workX * cosf(angle) - workY * sinf(angle) + workPosX;
				data[i].position.y = workX * sinf(angle) + workY * cosf(angle) + workPosY;
				data[i].position.z = 0.0f;
			}

			// 正規化デバイス座標系
			for (int i = 0; i < 4; i++) {
				data[i].position.x = 2.0f * data[i].position.x / screenWidth - 1.0f;
				data[i].position.y = 1.0f - 2.0f * data[i].position.y / screenHeight;
				data[i].position.z = 0.0f;
			}

			//テクスチャ座標設定
			data[0].texcoord.x = sx;
			data[0].texcoord.y = sy;
			data[1].texcoord.x = sx + sw;
			data[1].texcoord.y = sy;
			data[2].texcoord.x = sx;
			data[2].texcoord.y = sy + sh;
			data[3].texcoord.x = sx + sw;
			data[3].texcoord.y = sy + sh;

			//UV座標
			for (int i = 0; i < 4; i++) {
				data[i].texcoord.x = data[i].texcoord.x / textureWidth; // dss->GetWidth(1);
				data[i].texcoord.y = data[i].texcoord.y / textureHeight; // dss->GetHeight(1);
			}

			//頂点カラー
			data[0].color = { r, g, b, a };
			data[1].color = { r, g, b, a };
			data[2].color = { r, g, b, a };
			data[3].color = { r, g, b, a };

			//法線
			//data[0].Normal = XMFLOAT3(0, 0, 1);
			//data[1].Normal = XMFLOAT3(0, 0, 1);
			//data[2].Normal = XMFLOAT3(0, 0, 1);
			//data[3].Normal = XMFLOAT3(0, 0, 1);

			//頂点データ更新
			dc->UpdateSubresource(buffer.Get(), 0, NULL, data, 0, 0);

			//	頂点バッファの指定
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(
				0, 1, buffer.GetAddressOf(), // スロット, 数, バッファ
				&stride,		// １頂点のサイズ
				&offset			// 開始位置
			);
			dc->IASetPrimitiveTopology(
				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
			);

			dc->OMSetDepthStencilState(depthStencilState.Get(), 1);

			dc->Draw(4, 0);

			//シェーダ無効か
			dc->VSSetShader(NULL, NULL, 0);
			dc->PSSetShader(NULL, NULL, 0);

		}
	}
}