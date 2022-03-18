#include "Lib/Misc.h"
#include "Lib/Graphics/ShadowShader.h"
#include "Lib/Graphics/Graphics.h"
#include <assert.h>
#include <d3d11.h>
#include <memory>

namespace PP
{
	namespace Class
	{
		DepthShadowShader::DepthShadowShader(ID3D11Device* device)
		{
			// 頂点シェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\DepthShadowVS.cso", "rb");
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
					{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ピクセルシェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\DepthShadowPS.cso", "rb");
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

			// 定数バッファ
			{
				// シーン用バッファ
				D3D11_BUFFER_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.ByteWidth = sizeof(CbScene);
				desc.StructureByteStride = 0;

				HRESULT hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// メッシュ用バッファ
				desc.ByteWidth = sizeof(CbMesh);

				hr = device->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// サブセット用バッファ
				desc.ByteWidth = sizeof(CbSubset);

				hr = device->CreateBuffer(&desc, 0, subsetConstantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// ブラー用バッファ
				desc.ByteWidth = sizeof(ConstantBufferForPerFrameGaussianBlur);

				hr = device->CreateBuffer(&desc, 0, ConstantBufferGaussianBlur.GetAddressOf());
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

				HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 深度ステンシルステート
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.DepthEnable = true;
				desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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
				desc.CullMode = D3D11_CULL_BACK;
				desc.AntialiasedLineEnable = false;

				HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// サンプラステート
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
				desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

				HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			shadowConstantBuffer = std::make_shared<ConstantBufferForShadow>();
			gaussShader = std::make_unique<GaussShader>(device);

		}

		void DepthShadowShader::CalcGaussianFilter(DirectX::XMFLOAT4* array, int karnel_size, float sigma)
		{
			float sum = 0.0f;
			int id = 0;
			for (int y = -karnel_size / 2; y <= karnel_size / 2; y++)
			{
				for (int x = -karnel_size / 2; x <= karnel_size / 2; x++)
				{
					array[id].x = (float)x;
					array[id].y = (float)y;
					float pi2 = 2.0f * 3.141525653f;
					float A = 1 / (pi2 * pow(sigma, 2.0f));
					float B = exp(-((pow(array[id].x, 2.0f) + pow(array[id].y, 2.0f)) / (2.0f * pow(sigma, 2.0f))));

					array[id].z = A * B;

					sum += array[id].z;

					id++;
				}
			}

			for (int i = 0; i < karnel_size * karnel_size; i++)
			{
				array[i].z /= sum;
			}
		}

		bool DepthShadowShader::CreateDepth(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d = nullptr;
			HRESULT hr = S_OK;
			// テクスチャ作成
			::memset(&texture2d_desc[0], 0, sizeof(texture2d_desc[0]));
			texture2d_desc[0].Width = width;
			texture2d_desc[0].Height = height;
			texture2d_desc[0].MipLevels = 1;
			texture2d_desc[0].ArraySize = 1;
			texture2d_desc[0].Format = format;
			texture2d_desc[0].SampleDesc.Count = 1;
			texture2d_desc[0].SampleDesc.Quality = 0;
			texture2d_desc[0].Usage = D3D11_USAGE_DEFAULT;
			texture2d_desc[0].CPUAccessFlags = 0;
			texture2d_desc[0].MiscFlags = 0;
			texture2d_desc[0].BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

			hr = device->CreateTexture2D(&texture2d_desc[0], NULL, texture2d.GetAddressOf());
			assert(SUCCEEDED(hr));

			// 深度ステンシルビュー設定
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
			::memset(&dsvd, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvd.Texture2D.MipSlice = 0;
			hr = device->CreateDepthStencilView(texture2d.Get(), &dsvd, DepthStencilView.GetAddressOf());
			assert(SUCCEEDED(hr));

			//	シェーダーリソースビュー作成
			//D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
			//ZeroMemory(&srvd, sizeof(srvd));
			//srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			//srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			//srvd.Texture2D.MostDetailedMip = 0;
			//srvd.Texture2D.MipLevels = 1;
			//hr = device->CreateShaderResourceView(texture2d.Get(), &srvd, ShaderResourceView[0].GetAddressOf());
			//assert(SUCCEEDED(hr));

			return true;
		}

		bool DepthShadowShader::CreateRenderTarget(ID3D11Device* device, u_int width, u_int height, DXGI_FORMAT format)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d[2] = {};
			HRESULT hr = S_OK;
			// テクスチャ作成
			::memset(&texture2d_desc[1], 0, sizeof(texture2d_desc[1]));
			texture2d_desc[1].Width = width;
			texture2d_desc[1].Height = height;
			texture2d_desc[1].MipLevels = 1;
			texture2d_desc[1].ArraySize = 1;
			texture2d_desc[1].Format = format;
			texture2d_desc[1].SampleDesc.Count = 1;
			texture2d_desc[1].Usage = D3D11_USAGE_DEFAULT;
			texture2d_desc[1].CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			texture2d_desc[1].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

			hr = device->CreateTexture2D(&texture2d_desc[1], NULL, texture2d[0].GetAddressOf());
			assert(SUCCEEDED(hr));

			//	レンダーターゲットビュー作成
			D3D11_RENDER_TARGET_VIEW_DESC rtvd1;
			::memset(&rtvd1, 0, sizeof(rtvd1));
			rtvd1.Format = format;
			rtvd1.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvd1.Texture2D.MipSlice = 0;
			hr = device->CreateRenderTargetView(texture2d[0].Get(), &rtvd1, RenderTargetView[0].GetAddressOf());

			//	シェーダーリソースビュー作成
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd1;
			memset(&srvd1, 0, sizeof(srvd1));
			srvd1.Format = format;
			srvd1.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd1.Texture2D.MostDetailedMip = 0;
			srvd1.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(texture2d[0].Get(), &srvd1, ShaderResourceView[0].GetAddressOf());

			{
				::memset(&texture2d_desc[1], 0, sizeof(texture2d_desc[1]));
				texture2d_desc[1].Width = width;
				texture2d_desc[1].Height = height;
				texture2d_desc[1].MipLevels = 1;
				texture2d_desc[1].ArraySize = 1;
				texture2d_desc[1].Format = format;
				texture2d_desc[1].SampleDesc.Count = 1;
				texture2d_desc[1].Usage = D3D11_USAGE_DEFAULT;
				texture2d_desc[1].CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				texture2d_desc[1].BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

				hr = device->CreateTexture2D(&texture2d_desc[1], NULL, texture2d[1].GetAddressOf());
				assert(SUCCEEDED(hr));
			}

			{
				D3D11_RENDER_TARGET_VIEW_DESC rtvd2;
				::memset(&rtvd2, 0, sizeof(rtvd2));
				rtvd2.Format = format;
				rtvd2.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				rtvd2.Texture2D.MipSlice = 0;
				hr = device->CreateRenderTargetView(texture2d[1].Get(), &rtvd2, RenderTargetView[1].GetAddressOf());
				assert(SUCCEEDED(hr));
			}

			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srvd2;
				::memset(&srvd2, 0, sizeof(srvd2));
				srvd2.Format = format;
				srvd2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvd2.Texture2D.MostDetailedMip = 0;
				srvd2.Texture2D.MipLevels = 1;
				hr = device->CreateShaderResourceView(texture2d[1].Get(), &srvd2, ShaderResourceView[1].GetAddressOf());
				assert(SUCCEEDED(hr));
			}

			gaussShader->SetTextureSize(width, height);

			return true;
		}

		RenderContext& DepthShadowShader::ShadowContext()
		{
			DirectX::XMFLOAT3 sEye = Graphics::Instance().GetLight()->GetPosition();
			DirectX::XMFLOAT3 sTarget = shadowTarget;
			DirectX::XMFLOAT3 sUp = { 0, 1, 0 };

			DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&sEye);
			DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&sTarget);
			DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&sUp);

			RenderContext Rc = {};

			DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
			DirectX::XMStoreFloat4x4(&Rc.view, View);
			DirectX::XMMATRIX Projection = DirectX::XMMatrixOrthographicLH(50.0f, 50.0f, 0.1f, 100.0f);
			DirectX::XMStoreFloat4x4(&Rc.projection, Projection);
			return Rc;
		}

		// 描画開始
		void DepthShadowShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
		{
			RenderContext Rc = ShadowContext(); // 行列の上書き

			ID3D11RenderTargetView* rtv = GetRenderTargetView(0);
			ID3D11DepthStencilView* dsv = GetDepthStencilView();

			dc->OMSetRenderTargets(1, &rtv, dsv);
			float clearColor[4] = { 1, 1, 1, 1 };
			dc->ClearRenderTargetView(rtv, clearColor);
			dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)texture2d_desc[0].Width;
			vp.Height = (FLOAT)texture2d_desc[0].Height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			dc->RSSetViewports(1, &vp);

			dc->VSSetShader(vertexShader.Get(), nullptr, 0);
			dc->PSSetShader(pixelShader.Get(), nullptr, 0);
			dc->IASetInputLayout(inputLayout.Get());

			ID3D11Buffer* constantBuffers[] =
			{
				sceneConstantBuffer.Get(),
				meshConstantBuffer.Get(),
				subsetConstantBuffer.Get(),
				ConstantBufferGaussianBlur.Get()
			};
			dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
			dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

			const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
			dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
			dc->RSSetState(rasterizerState.Get());
			dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

			// シーン用定数バッファ更新
			CbScene cbScene;

			DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&Rc.view);
			DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&Rc.projection);
			DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

			cbScene.lightDirection = Graphics::Instance().GetLight()->GetDirection();
			dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);

			DirectX::XMStoreFloat4x4(&GetShadowConstantBuffer()->LightViewProjection, V * P);
			GetShadowConstantBuffer()->ShadowColor = { 0.5f, 0.5f, 0.5f, 0.25f };
		}

		// 描画
		void DepthShadowShader::Draw(ID3D11DeviceContext* dc, const LibModel* model)
		{
			const ModelResource* resource = model->GetResource();
			const std::vector<LibModel::Node>& nodes = model->GetNodes();

			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				// メッシュ用定数バッファ更新
				CbMesh cbMesh;
				::memset(&cbMesh, 0, sizeof(cbMesh));
				if (mesh.nodeIndices.size() > 0)
				{
					for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
					{
						DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
						DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
						DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
						DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[i], boneTransform);
					}
				}
				else
				{
					cbMesh.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
				}
				dc->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

				UINT stride = sizeof(ModelResource::Vertex);
				UINT offset = 0;
				dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				//dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				for (const ModelResource::Subset& subset : mesh.subsets)
				{
					//CbSubset cbSubset;
					//cbSubset.materialColor = subset.material->color;
					//dc->UpdateSubresource(subsetConstantBuffer.Get(), 0, 0, &cbSubset, 0, 0);
					//dc->PSSetShaderResources(0, 1, subset.material->shaderResourceView.GetAddressOf());
					//dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());
					dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
				}
			}
			

		}

		// 描画終了
		void DepthShadowShader::End(ID3D11DeviceContext* dc)
		{
			// ぼかし処理？？？
			{
				{
					// サンプリングの位置オフセットとウェイトを計算
					DirectX::XMFLOAT4 weight[BUFFER_SIZE];
					ZeroMemory(weight, sizeof(weight));
					CalcGaussianFilter(weight, 5, 50);
					// コンスタントバッファ更新
					ConstantBufferForPerFrameGaussianBlur cb;
					memcpy(cb.Weight, weight, sizeof(DirectX::XMFLOAT4) * BUFFER_SIZE);
					cb.KarnelSize = 5;
					cb.texcel.x = 1.0f / static_cast<float>(texture2d_desc[0].Width);
					cb.texcel.y = 1.0f / static_cast<float>(texture2d_desc[0].Height);
					dc->UpdateSubresource(ConstantBufferGaussianBlur.Get(), 0, NULL, &cb, 0, 0);
					//dc->VSSetConstantBuffers(3, 1, ConstantBufferGaussianBlur.GetAddressOf());
					//dc->PSSetConstantBuffers(3, 1, ConstantBufferGaussianBlur.GetAddressOf());
				}
				ID3D11RenderTargetView* rtv = GetRenderTargetView(1);
				dc->OMSetRenderTargets(1, &rtv, nullptr);
				// 画面クリア
				float clearColor[4] = { 0, 0, 0 ,0 };
				dc->ClearRenderTargetView(rtv, clearColor);
				// ビューポート設定
				//D3D11_VIEWPORT vp;
				//vp.Width = (FLOAT)texture2d_desc[0].Width;
				//vp.Height = (FLOAT)texture2d_desc[0].Height;
				//vp.MinDepth = 0.0f;
				//vp.MaxDepth = 1.0f;
				//vp.TopLeftX = 0;
				//vp.TopLeftY = 0;
				//dc->RSSetViewports(1, &vp);
				//const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				//dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
				//dc->OMSetDepthStencilState(depthStencilState.Get(), 0);
				//dc->RSSetState(rasterizerState.Get());
				//dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());
				dc->PSSetShaderResources(0, 1, ShaderResourceView[0].GetAddressOf());
				// ブラー
				gaussShader->Render(dc,
					0, 0, Graphics::Instance().GetScreenWidth(), Graphics::Instance().GetScreenHeight(),
					0, 0, texture2d_desc[0].Width, texture2d_desc[0].Height,
					0,
					1.0f, 1.0f, 1.0f, 1.0f);
			}

			// メインのレンダーターゲットにもどす
			ID3D11RenderTargetView* backbuffer = Graphics::Instance().GetRenderTargetView();
			dc->OMSetRenderTargets(1, &backbuffer, Graphics::Instance().GetDepthStencilView());

			dc->VSSetShader(nullptr, nullptr, 0);
			dc->PSSetShader(nullptr, nullptr, 0);
			dc->IASetInputLayout(nullptr);
		}



		RenderShadowShader::RenderShadowShader(ID3D11Device* device)
		{
			// 頂点シェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\RenderShadowVS.cso", "rb");
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
					{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), csoData.get(), csoSize, inputLayout.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// ピクセルシェーダー
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\RenderShadowPS.cso", "rb");
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

			// 定数バッファ
			{
				// シーン用バッファ
				D3D11_BUFFER_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;
				desc.ByteWidth = sizeof(CbScene);
				desc.StructureByteStride = 0;

				HRESULT hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// メッシュ用バッファ
				desc.ByteWidth = sizeof(CbMesh);

				hr = device->CreateBuffer(&desc, 0, meshConstantBuffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

				// サブセット用バッファ
				desc.ByteWidth = sizeof(CbSubset);

				hr = device->CreateBuffer(&desc, 0, subsetConstantBuffer.GetAddressOf());
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

				HRESULT hr = device->CreateBlendState(&desc, blendState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// 深度ステンシルステート
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				::memset(&desc, 0, sizeof(desc));
				desc.DepthEnable = true;
				desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
				desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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
				desc.CullMode = D3D11_CULL_BACK;
				desc.AntialiasedLineEnable = false;

				HRESULT hr = device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			// サンプラステート
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
				desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

				HRESULT hr = device->CreateSamplerState(&desc, samplerState.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
			}

			int size = 1024;
			depthShadow = std::make_shared<DepthShadowShader>(device);
			depthShadow->CreateDepth(device, size, size);
			depthShadow->CreateRenderTarget(device, size, size, DXGI_FORMAT_R32G32_FLOAT);

		}

		// 描画開始
		void RenderShadowShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
		{
			//Graphics::Instance().ScreenClear(); // 画面クリア

			D3D11_VIEWPORT vp;
			vp.Width = Graphics::Instance().GetScreenWidth();
			vp.Height = Graphics::Instance().GetScreenHeight(); 
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			dc->RSSetViewports(1, &vp);

			dc->VSSetShader(vertexShader.Get(), nullptr, 0);
			dc->PSSetShader(pixelShader.Get(), nullptr, 0);
			dc->IASetInputLayout(inputLayout.Get());
			
			ID3D11Buffer* constantBuffers[] =
			{
				sceneConstantBuffer.Get(),
				meshConstantBuffer.Get(),
				subsetConstantBuffer.Get()
			};
			dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
			dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

			// DepthShadowで書いてることと同じ？やからなくてもいい？
			//const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			//dc->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
			//dc->RSSetState(rasterizerState.Get());
			//dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());
			dc->OMSetDepthStencilState(depthStencilState.Get(), 0);

			// シーン用定数バッファ更新
			CbScene cbScene;

			DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
			DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
			DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);

			cbScene.lightDirection = Graphics::Instance().GetLight()->GetDirection();
			dc->UpdateSubresource(sceneConstantBuffer.Get(), 0, 0, &cbScene, 0, 0);
		}

		// 描画
		void RenderShadowShader::Draw(ID3D11DeviceContext* dc, const LibModel* model)
		{
			const ModelResource* resource = model->GetResource();
			const std::vector<LibModel::Node>& nodes = model->GetNodes();

			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				// メッシュ用定数バッファ更新
				CbMesh cbMesh;
				::memset(&cbMesh, 0, sizeof(cbMesh));
				if (mesh.nodeIndices.size() > 0)
				{
					for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
					{
						DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
						DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
						DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
						DirectX::XMStoreFloat4x4(&cbMesh.boneTransforms[i], boneTransform);
					}
				}
				else
				{
					cbMesh.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
				}
				dc->UpdateSubresource(meshConstantBuffer.Get(), 0, 0, &cbMesh, 0, 0);

				UINT stride = sizeof(ModelResource::Vertex);
				UINT offset = 0;
				dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
				dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
				dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				ID3D11ShaderResourceView* srv = depthShadow.get()->GetShaderResourceView(1);
				dc->PSSetSamplers(3, 1, samplerState.GetAddressOf());
				dc->PSSetShaderResources(3, 1, &srv);

				LibLight* light = Graphics::Instance().GetLight();
				for (const ModelResource::Subset& subset : mesh.subsets)
				{
					CbSubset cbSubset;
					cbSubset.materialColor = subset.material->color;
					cbSubset.AmbientColor = light->GetAmbient();
					cbSubset.LightColor = light->GetColor(); 
					cbSubset.EyePos = { light->GetPosition().x, light->GetPosition().y, light->GetPosition().z, 1 }; //どこの位置がいいの?
					cbSubset.LightViewProjection = depthShadow->GetShadowConstantBuffer()->LightViewProjection;
					cbSubset.ShadowColor = depthShadow->GetShadowConstantBuffer()->ShadowColor;
					//cbSubset.ShadowColor = { 0, 1, 0, 1 };
					dc->UpdateSubresource(subsetConstantBuffer.Get(), 0, 0, &cbSubset, 0, 0);
					dc->PSSetShaderResources(0, 1, subset.material->shaderResourceView.GetAddressOf());
					//dc->PSSetSamplers(0, 1, samplerState.GetAddressOf());

					dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
				}
			}

		}

		// 描画終了
		void RenderShadowShader::End(ID3D11DeviceContext* dc)
		{
			ID3D11ShaderResourceView* srv[1] = { NULL };
			dc->PSSetShaderResources(3, 1, srv);

			dc->VSSetShader(nullptr, nullptr, 0);
			dc->PSSetShader(nullptr, nullptr, 0);
			dc->IASetInputLayout(nullptr);
		}
	}
}