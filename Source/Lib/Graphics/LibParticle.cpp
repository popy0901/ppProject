#include <WICTextureLoader.h>
#include "Lib/Graphics/LibParticle.h"
#include "Lib/Graphics/Graphics.h"
#include "Lib/Misc.h"

namespace PP
{
	namespace Class
	{
		LibParticle::LibParticle(const char* fileName, int num)
		{
			ID3D11Device* device = Graphics::Instance().GetDevice();

			data = new Data[num];
			memset(data, 0, sizeof(Data) * num);
			vertex = new Vertex[num];
			memset(vertex, 0, sizeof(Vertex) * num);

			this->num = num;

			for (int i = 0; i < this->num; i++) { data[i].type = -1; }

			// 頂点シェーダ
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\ParticleVS.cso", "rb");
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
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,	 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ジオメトリシェーダ
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\ParticleGS.cso", "rb");
				_ASSERT_EXPR_A(fp, "CSO File not found");

				// ファイルのサイズを求める
				fseek(fp, 0, SEEK_END);
				long csoSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				// メモリ上に頂点シェーダーデータを格納する領域を用意する
				std::unique_ptr<u_char[]> csoData = std::make_unique<u_char[]>(csoSize);
				fread(csoData.get(), csoSize, 1, fp);
				fclose(fp);

				// ジオメトリシェーダー生成
				HRESULT hr = device->CreateGeometryShader(csoData.get(), csoSize, nullptr, geometryShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ピクセルシェーダ
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\ParticlePS.cso", "rb");
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
				HRESULT hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// シェーダリソースビュー
			{
				// マルチバイト文字からワイド文字へ変換
				wchar_t wfileName[256];
				::MultiByteToWideChar(CP_ACP, 0, fileName, -1, wfileName, 256);

				// テクスチャファイル読み込み
				// テクスチャ読み込み
				Microsoft::WRL::ComPtr<ID3D11Resource> resource = nullptr;
				HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfileName, resource.GetAddressOf(), shaderResourceView.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// テクスチャ情報の取得
				D3D11_TEXTURE2D_DESC desc;
				Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d = nullptr;
				hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
				texture2d->GetDesc(&desc);

				//textureWidth = desc.Width;
				//textureHeight = desc.Height;
			}

			// 頂点バッファ
			{
				D3D11_BUFFER_DESC bd;
				::memset(&bd, 0, sizeof(bd));
				bd.Usage = D3D11_USAGE_DEFAULT;
				//頂点数分のバッファ
				bd.ByteWidth = sizeof(Vertex) * this->num;
				bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags = 0;

				HRESULT hr = device->CreateBuffer(&bd, NULL, vertexBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 定数バッファ
			{
				D3D11_BUFFER_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.ByteWidth = sizeof(ConstantBufferForPerFrame);
				desc.StructureByteStride = 0;

				HRESULT hr = device->CreateBuffer(&desc, nullptr, constantBuffer.GetAddressOf());
				assert(SUCCEEDED(hr));
			}

			// ブレンド // 仮置き(ALPHA)
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

				HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 深度ステンシルステート // 描画準に気を付けよう
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.DepthEnable = false;
				HRESULT hr = device->CreateDepthStencilState(&desc, depthStencilState[0].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				::memset(&desc, 0, sizeof(desc));
				desc.DepthEnable = true;
				//desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				//desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
				desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				desc.DepthFunc = D3D11_COMPARISON_LESS;

				hr = device->CreateDepthStencilState(&desc, depthStencilState[1].GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ラスタライザー
			{
				D3D11_RASTERIZER_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.FrontCounterClockwise = false;
				desc.DepthBias = 0;
				desc.DepthBiasClamp = 0;
				desc.SlopeScaledDepthBias = 0;
				desc.DepthClipEnable = true;
				desc.ScissorEnable = false;
				desc.MultisampleEnable = false;
				desc.FillMode = D3D11_FILL_SOLID;
				desc.CullMode = D3D11_CULL_BACK;
				desc.AntialiasedLineEnable = false;

				HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// サンプラー
			{
				D3D11_SAMPLER_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.MipLODBias = 0.0f;
				desc.MaxAnisotropy = 1;
				desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
				desc.MinLOD = -FLT_MAX;
				desc.MaxLOD = FLT_MAX;
				desc.BorderColor[0] = 1.0f;
				desc.BorderColor[1] = 1.0f;
				desc.BorderColor[2] = 1.0f;
				desc.BorderColor[3] = 1.0f;
				desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

				HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

		}

		void LibParticle::Update(float elapsedTime)
		{
			float time = elapsedTime;
			for (int i = 0; i < num; i++) 
			{
				if (data[i].type < 0) continue;
				data[i].vx += data[i].ax * time;
				data[i].vy += data[i].ay * time;
				data[i].vz += data[i].az * time;
				data[i].x += data[i].vx * time;
				data[i].y += data[i].vy * time;
				data[i].z += data[i].vz * time;
				data[i].timer -= time;
				data[i].alpha = data[i].timer / 0.2f;
				data[i].scale = 1.0f; // 通常の大きさ
				data[i].angle += 0.1f;
				if (data[i].timer <= 0) data[i].type = -1;
			}
		}

		void LibParticle::Render(ID3D11DeviceContext* dc, const RenderContext& rc)
		{
			dc->VSSetShader(vertexShader.Get(), nullptr, 0);
			dc->GSSetShader(geometryShader.Get(), nullptr, 0);
			dc->PSSetShader(pixelShader.Get(), nullptr, 0);
			dc->IASetInputLayout(inputLayout.Get());

			ConstantBufferForPerFrame cb;
			cb.view = rc.view;
			cb.projection = rc.projection;

			dc->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
			dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			dc->GSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			dc->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			dc->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
			dc->RSSetState(rasterizerState.Get());
			dc->OMSetDepthStencilState(depthStencilState[1].Get(), 1);

			dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			int n = 0; //パーティクル発生数
			for (int i = 0; i < num; i++)
			{
				if (data[i].type < 0) continue;
				vertex[n].Position.x = data[i].x;
				vertex[n].Position.y = data[i].y;
				vertex[n].Position.z = data[i].z;
				vertex[n].Tex.x = data[i].w;
				vertex[n].Tex.y = data[i].h;
				vertex[n].Normal.x = (float)data[i].type;
				vertex[n].Normal.y = data[i].scale;
				vertex[n].Normal.z = data[i].angle;
				vertex[n].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
				vertex[n].Color.w = data[i].alpha;
				++n;
			}

			dc->UpdateSubresource(vertexBuffer.Get(), 0, nullptr, vertex, 0, 0);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

			dc->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
			dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

			dc->Draw(n, 0);

			dc->OMSetDepthStencilState(depthStencilState[0].Get(), 1);

			dc->VSSetShader(nullptr, nullptr, 0);
			dc->GSSetShader(nullptr, nullptr, 0);
			dc->PSSetShader(nullptr, nullptr, 0);
			dc->IASetInputLayout(nullptr);
		}

		void LibParticle::Animation(float elapsedTime, float speed)
		{
			float time =elapsedTime;
			for (int i = 0; i < num; i++) 
			{
				if (data[i].type < 0) continue;

				data[i].anime_timer += time;
				if (data[i].anime_timer > speed) 
				{
					data[i].anime_timer = 0.0f;
					data[i].type = (data[i].type + 1) % 6;
				}

			}
		}

		void LibParticle::Set(int type, float timer,
			DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v,
			DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size)
		{
			for (int i = 0; i < num; i++) 
			{
				if (data[i].type >= 0) continue;
				data[i].type = type;
				data[i].x = p.x;
				data[i].y = p.y;
				data[i].z = p.z;
				data[i].vx = v.x;
				data[i].vy = v.y;
				data[i].vz = v.z;
				data[i].ax = f.x;
				data[i].ay = f.y;
				data[i].az = f.z;
				data[i].w = size.x;
				data[i].h = size.y;
				data[i].alpha = 1.0f;
				data[i].timer = timer;
				break;
			}
		}

		void LibParticle::Snow(DirectX::XMFLOAT3 pos, int max)
		{
			for (int i = 0; i < max; i++)
			{
				//発生位置
				DirectX::XMFLOAT3 p = { 0,0,0 };
				p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
				p.y = pos.y;
				p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
				//発生方向
				DirectX::XMFLOAT3 v = { 0,0,0 };
				v.y = -(rand() % 10001) * 0.0002f - 0.002f;
				//力
				DirectX::XMFLOAT3 f = { 0,0,0 };
				f.x = (rand() % 10001) * 0.00001f + 0.1f;
				f.z = (rand() % 10001 - 5000) * 0.00001f;
				//大きさ
				DirectX::XMFLOAT2 s = { .2f,.2f };

				Set(12, 5, p, v, f, s);
			}
		}

		void LibParticle::Spark(DirectX::XMFLOAT3 pos, int max)
		{
			for (int i = 0; i < max; i++) 
			{
				DirectX::XMFLOAT3 p;
				p.x = pos.x;
				p.y = pos.y;
				p.z = pos.z;

				DirectX::XMFLOAT3 v = { 0,0,0 };
				v.x = (rand() % 10001 - 5000) * 0.0001f;
				v.y = (rand() % 10001) * 0.0002f + 1.2f;
				v.z = (rand() % 10001 - 5000) * 0.0001f;

				DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
				DirectX::XMFLOAT2 s = { 0.05f,0.05f };

				Set(2, 3, p, v, f, s);

			}
		}

		void LibParticle::Fire(DirectX::XMFLOAT3 pos, int max)
		{
			for (int i = 0; i < max; i++) 
			{
				DirectX::XMFLOAT3 p;
				DirectX::XMFLOAT3 v = { 0,0,0 };
				DirectX::XMFLOAT3 f = { 0,-2.0f,0 };
				DirectX::XMFLOAT2 s = { .4f,.4f };

				p.x = pos.x + (rand() % 10001 - 5000) * 0.00005f;
				p.y = pos.y + (rand() % 10001) * 0.0001f + 0.2f;
				p.z = pos.z + (rand() % 10001 - 5000) * 0.00005f;

				v.x = (rand() % 10001 - 5000) * 0.0002f;
				v.y = (rand() % 10001) * 0.0004f + 0.005f;
				v.z = (rand() % 10001 - 5000) * 0.0002f;

				Set(13, 0.4f, p, v, f, s);

			}
		}

		void LibParticle::Smoke(DirectX::XMFLOAT3 pos, int max)
		{
			for (int i = 0; i < max; i++) 
			{
				DirectX::XMFLOAT3 p;
				DirectX::XMFLOAT3 v = { 0,0,0 };
				DirectX::XMFLOAT3 f = { 0,-2.0f,0 };
				DirectX::XMFLOAT2 s = { .4f,.4f };

				p.x = pos.x + (rand() % 10001 - 5000) * 0.0001f;
				p.y = pos.y + (rand() % 10001) * 0.0001f + 0.5f;
				p.z = pos.z + (rand() % 10001 - 5000) * 0.0001f;

				v.x = (rand() % 10001 - 5000) * 0.0001f;
				v.y = (rand() % 10001) * 0.0004f + 0.005f;
				v.z = (rand() % 10001 - 5000) * 0.0001f;
				Set(14, 0.6f, p, v, f, s);

			}
		}

	}
}