#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <WICTextureLoader.h>
#include "Lib/Graphics/Pfc.h"
#include "Lib/Lib.h"
#include "Lib/Graphics/Graphics.h"
#include "Lib/Logger.h"
#include "Lib/Misc.h"

CEREAL_CLASS_VERSION(PP::Class::LibEfc::Save, 1);
CEREAL_CLASS_VERSION(PP::Class::LibEfc, 1);

namespace DirectX
{
	template<class Archive>
	void serialize(Archive& archive, XMUINT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class Archive>
	void serialize(Archive& archive, XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

namespace PP
{
	namespace Class
	{
		template<class Archive>
		void LibEfc::Save::serialize(Archive& archive, int version)
		{
			archive(
				CEREAL_NVP(centerPos),
				CEREAL_NVP(centerRandomPos),
				CEREAL_NVP(velocityPos),
				CEREAL_NVP(velocityRandomPos),
				CEREAL_NVP(accelPos),
				CEREAL_NVP(accelRandomPos),
				CEREAL_NVP(easingStartPos),
				CEREAL_NVP(easingRandomStartPos),
				CEREAL_NVP(easingEndPos),
				CEREAL_NVP(easingRandomEndPos),
				CEREAL_NVP(radius),
				CEREAL_NVP(rotation),
				CEREAL_NVP(angle),
				CEREAL_NVP(spin),
				CEREAL_NVP(centerScale),
				CEREAL_NVP(centerRandomScale),
				CEREAL_NVP(easingStartScale),
				CEREAL_NVP(easingRandomStartScale),
				CEREAL_NVP(easingEndScale),
				CEREAL_NVP(easingRandomEndScale),
				CEREAL_NVP(centerColor),
				CEREAL_NVP(centerRandomColor),
				CEREAL_NVP(easingStartColor),
				CEREAL_NVP(easingRandomStartColor),
				CEREAL_NVP(easingEndColor),
				CEREAL_NVP(easingRandomEndColor),
				CEREAL_NVP(totalTime),
				CEREAL_NVP(existTimer),
				CEREAL_NVP(makeTimer),
				CEREAL_NVP(index),
				CEREAL_NVP(fileName),
				CEREAL_NVP(Axis),
				CEREAL_NVP(easingF)
			);
		}

		LibEfc::LibEfc(const char* fileName)
		{
			save.clear();
			save.resize(1);

			// デシリアライズ
			std::ifstream istream(fileName, std::ios::binary);
			if (istream.is_open())
			{
				cereal::BinaryInputArchive archive(istream);

				try
				{
					archive(CEREAL_NVP(save));
				}
				catch (...)
				{
					LOG("pfc deserialize failed.\n%s\n", fileName);
					return;
				}
			}
			Create();
		}

		void LibEfc::Create()
		{
			ID3D11Device* device = Graphics::Instance().GetDevice();

			for (Save& s : save)
			{
				*s.fileName = *save.at(0).fileName;
				LoadTexture(s.fileName);
				*s.centerPos = *save.at(0).centerPos;
				*s.centerRandomPos = *save.at(0).centerRandomPos;
				*s.velocityPos = *save.at(0).velocityPos;
				*s.velocityRandomPos = *save.at(0).velocityRandomPos;
				*s.accelPos = *save.at(0).accelPos;
				*s.accelRandomPos = *save.at(0).accelRandomPos;
				*s.easingStartPos = *save.at(0).easingStartPos;
				*s.easingRandomStartPos = *save.at(0).easingRandomStartPos;
				*s.easingEndPos = *save.at(0).easingEndPos;
				*s.easingRandomEndPos = *save.at(0).easingRandomEndPos;

				s.radius = save.at(0).radius;
				s.rotation = save.at(0).rotation;
				s.angle = save.at(0).angle;
				s.spin = save.at(0).spin;

				*s.centerScale = *save.at(0).centerScale;
				*s.centerRandomScale = *save.at(0).centerRandomScale;
				*s.easingStartScale = *save.at(0).easingStartScale;
				*s.easingRandomStartScale = *save.at(0).easingRandomStartScale;
				*s.easingEndScale = *save.at(0).easingEndScale;
				*s.easingRandomEndScale = *save.at(0).easingRandomEndScale;

				*s.centerColor = *save.at(0).centerColor;
				*s.centerRandomColor = *save.at(0).centerRandomColor;
				*s.easingStartColor = *save.at(0).easingStartColor;
				*s.easingRandomStartColor = *save.at(0).easingRandomStartColor;
				*s.easingEndColor = *save.at(0).easingEndColor;
				*s.easingRandomEndColor = *save.at(0).easingRandomEndColor;

				*s.totalTime = *save.at(0).totalTime;
				*s.existTimer = *save.at(0).existTimer;
				s.makeTimer = save.at(0).makeTimer;
				num = s.makeTimer;
				*s.index = *save.at(0).index;
				*s.Axis = *save.at(0).Axis;
				*s.easingF = *save.at(0).easingF;
			}

			data.resize(num);
			param = new Param;
			vertex = new Vertex[num];
			memset(vertex, 0, sizeof(Vertex) * num);

			for (int i = 0; i < this->num; i++) { data[i].type = -1; }

			// 頂点シェーダ
			{
				// ファイルを開く
				FILE* fp = nullptr;
				fopen_s(&fp, "Shader\\Pfc_VS.cso", "rb");
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
				fopen_s(&fp, "Shader\\Pfc_GS.cso", "rb");
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
				fopen_s(&fp, "Shader\\Pfc_PS.cso", "rb");
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

			// ブレンド 
			{
				D3D11_BLEND_DESC desc;
				// ADD
				::memset(&desc, 0, sizeof(desc));
				desc.AlphaToCoverageEnable = false;
				desc.IndependentBlendEnable = false;
				desc.RenderTarget[0].BlendEnable = true;
				desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
				desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
				desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
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
				desc.CullMode = D3D11_CULL_NONE;
				//desc.CullMode = D3D11_CULL_BACK;
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

		void LibEfc::LoadTexture(const char* fileName)
		{
			ID3D11Device* device = Graphics::Instance().GetDevice();
			if (shaderResourceView != nullptr) shaderResourceView = nullptr;
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
			}
		}

		void LibEfc::EaseSetPosition(int index, int num, float total)
		{
			switch (index)
			{
				// Sine
				{
			case 0:
				data[num].px = PP::Easing::EaseInSine(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInSine(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInSine(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 1:
				data[num].px = PP::Easing::EaseOutSine(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutSine(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutSine(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 2:
				data[num].px = PP::Easing::EaseInOutSine(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutSine(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutSine(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Quad
				{
			case 3:
				data[num].px = PP::Easing::EaseInQuad(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInQuad(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInQuad(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 4:
				data[num].px = PP::Easing::EaseOutQuad(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutQuad(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutQuad(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 5:
				data[num].px = PP::Easing::EaseInOutQuad(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutQuad(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutQuad(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Cubic
				{
			case 6:
				data[num].px = PP::Easing::EaseInCubic(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInCubic(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInCubic(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 7:
				data[num].px = PP::Easing::EaseOutCubic(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutCubic(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutCubic(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 8:
				data[num].px = PP::Easing::EaseInOutCubic(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutCubic(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutCubic(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Quart
				{
			case 9:
				data[num].px = PP::Easing::EaseInQuart(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInQuart(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInQuart(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 10:
				data[num].px = PP::Easing::EaseOutQuart(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutQuart(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutQuart(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 11:
				data[num].px = PP::Easing::EaseInOutQuart(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutQuart(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutQuart(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Quint
				{
			case 12:
				data[num].px = PP::Easing::EaseInQuint(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInQuint(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInQuint(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 13:
				data[num].px = PP::Easing::EaseOutQuint(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutQuint(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutQuint(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 14:
				data[num].px = PP::Easing::EaseInOutQuint(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutQuint(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutQuint(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Expo
				{
			case 15:
				data[num].px = PP::Easing::EaseInExpo(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInExpo(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInExpo(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 16:
				data[num].px = PP::Easing::EaseOutExpo(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutExpo(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutExpo(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 17:
				data[num].px = PP::Easing::EaseInOutExpo(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutExpo(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutExpo(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Circ
				{
			case 18:
				data[num].px = PP::Easing::EaseInCirc(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInCirc(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInCirc(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 19:
				data[num].px = PP::Easing::EaseOutCirc(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutCirc(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutCirc(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 20:
				data[num].px = PP::Easing::EaseInOutCirc(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutCirc(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutCirc(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Back
				{
			case 21:
				data[num].px = PP::Easing::EaseInBack(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInBack(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInBack(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 22:
				data[num].px = PP::Easing::EaseOutBack(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutBack(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutBack(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 23:
				data[num].px = PP::Easing::EaseInOutBack(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutBack(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutBack(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Elastic
				{
			case 24:
				data[num].px = PP::Easing::EaseInElastic(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInElastic(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInElastic(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 25:
				data[num].px = PP::Easing::EaseOutElastic(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutElastic(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutElastic(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 26:
				data[num].px = PP::Easing::EaseInOutElastic(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutElastic(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutElastic(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}
				// Bounce
				{
			case 27:
				data[num].px = PP::Easing::EaseInBounce(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInBounce(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInBounce(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 28:
				data[num].px = PP::Easing::EaseOutBounce(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseOutBounce(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseOutBounce(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
			case 29:
				data[num].px = PP::Easing::EaseInOutBounce(data[num].px1, data[num].px2, data[num].eTimer1, total);
				data[num].py = PP::Easing::EaseInOutBounce(data[num].py1, data[num].py2, data[num].eTimer1, total);
				data[num].pz = PP::Easing::EaseInOutBounce(data[num].pz1, data[num].pz2, data[num].eTimer1, total);
				break;
				}

			}
		}

		void LibEfc::EaseSetScale(int index, int num, float total)
		{
			switch (index)
			{
				// Sine
				{
			case 0:
				data[num].w = PP::Easing::EaseInSine(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInSine(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 1:
				data[num].w = PP::Easing::EaseOutSine(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutSine(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 2:
				data[num].w = PP::Easing::EaseInOutSine(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutSine(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Quad
				{
			case 3:
				data[num].w = PP::Easing::EaseInQuad(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInQuad(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 4:
				data[num].w = PP::Easing::EaseOutQuad(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutQuad(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 5:
				data[num].w = PP::Easing::EaseInOutQuad(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutQuad(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Cubic
				{
			case 6:
				data[num].w = PP::Easing::EaseInCubic(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInCubic(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 7:
				data[num].w = PP::Easing::EaseOutCubic(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutCubic(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 8:
				data[num].w = PP::Easing::EaseInOutCubic(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutCubic(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Quart
				{
			case 9:
				data[num].w = PP::Easing::EaseInQuart(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInQuart(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 10:
				data[num].w = PP::Easing::EaseOutQuart(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutQuart(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 11:
				data[num].w = PP::Easing::EaseInOutQuart(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutQuart(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Quint
				{
			case 12:
				data[num].w = PP::Easing::EaseInQuint(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInQuint(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 13:
				data[num].w = PP::Easing::EaseOutQuint(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutQuint(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 14:
				data[num].w = PP::Easing::EaseInOutQuint(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutQuint(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Expo
				{
			case 15:
				data[num].w = PP::Easing::EaseInExpo(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInExpo(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 16:
				data[num].w = PP::Easing::EaseOutExpo(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutExpo(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 17:
				data[num].w = PP::Easing::EaseInOutExpo(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutExpo(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Circ
				{
			case 18:
				data[num].w = PP::Easing::EaseInCirc(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInCirc(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 19:
				data[num].w = PP::Easing::EaseOutCirc(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutCirc(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 20:
				data[num].w = PP::Easing::EaseInOutCirc(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutCirc(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Back
				{
			case 21:
				data[num].w = PP::Easing::EaseInBack(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInBack(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 22:
				data[num].w = PP::Easing::EaseOutBack(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutBack(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 23:
				data[num].w = PP::Easing::EaseInOutBack(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutBack(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Elastic
				{
			case 24:
				data[num].w = PP::Easing::EaseInElastic(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInElastic(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 25:
				data[num].w = PP::Easing::EaseOutElastic(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutElastic(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 26:
				data[num].w = PP::Easing::EaseInOutElastic(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutElastic(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}
				// Bounce
				{
			case 27:
				data[num].w = PP::Easing::EaseInBounce(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInBounce(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 28:
				data[num].w = PP::Easing::EaseOutBounce(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseOutBounce(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
			case 29:
				data[num].w = PP::Easing::EaseInOutBounce(data[num].w1, data[num].w2, data[num].eTimer2, total);
				data[num].h = PP::Easing::EaseInOutBounce(data[num].h1, data[num].h2, data[num].eTimer2, total);
				break;
				}

			}
		}

		void LibEfc::EaseSetColor(int index, int num, float total)
		{
			switch (index)
			{
				// Sine
				{
			case 0:
				data[num].r = PP::Easing::EaseInSine(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInSine(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInSine(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 1:
				data[num].r = PP::Easing::EaseOutSine(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutSine(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutSine(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 2:
				data[num].r = PP::Easing::EaseInOutSine(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutSine(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutSine(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Quad
				{
			case 3:
				data[num].r = PP::Easing::EaseInQuad(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInQuad(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInQuad(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 4:
				data[num].r = PP::Easing::EaseOutQuad(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutQuad(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutQuad(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 5:
				data[num].r = PP::Easing::EaseInOutQuad(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutQuad(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutQuad(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Cubic
				{
			case 6:
				data[num].r = PP::Easing::EaseInCubic(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInCubic(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInCubic(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 7:
				data[num].r = PP::Easing::EaseOutCubic(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutCubic(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutCubic(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 8:
				data[num].r = PP::Easing::EaseInOutCubic(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutCubic(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutCubic(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Quart
				{
			case 9:
				data[num].r = PP::Easing::EaseInQuart(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInQuart(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInQuart(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 10:
				data[num].r = PP::Easing::EaseOutQuart(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutQuart(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutQuart(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 11:
				data[num].r = PP::Easing::EaseInOutQuart(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutQuart(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutQuart(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Quint
				{
			case 12:
				data[num].r = PP::Easing::EaseInQuint(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInQuint(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInQuint(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 13:
				data[num].r = PP::Easing::EaseOutQuint(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutQuint(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutQuint(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 14:
				data[num].r = PP::Easing::EaseInOutQuint(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutQuint(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutQuint(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Expo
				{
			case 15:
				data[num].r = PP::Easing::EaseInExpo(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInExpo(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInExpo(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 16:
				data[num].r = PP::Easing::EaseOutExpo(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutExpo(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutExpo(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 17:
				data[num].r = PP::Easing::EaseInOutExpo(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutExpo(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutExpo(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Circ
				{
			case 18:
				data[num].r = PP::Easing::EaseInCirc(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInCirc(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInCirc(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 19:
				data[num].r = PP::Easing::EaseOutCirc(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutCirc(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutCirc(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 20:
				data[num].r = PP::Easing::EaseInOutCirc(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutCirc(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutCirc(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Back
				{
			case 21:
				data[num].r = PP::Easing::EaseInBack(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInBack(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInBack(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 22:
				data[num].r = PP::Easing::EaseOutBack(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutBack(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutBack(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 23:
				data[num].r = PP::Easing::EaseInOutBack(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutBack(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutBack(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Elastic
				{
			case 24:
				data[num].r = PP::Easing::EaseInElastic(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInElastic(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInElastic(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 25:
				data[num].r = PP::Easing::EaseOutElastic(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutElastic(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutElastic(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 26:
				data[num].r = PP::Easing::EaseInOutElastic(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutElastic(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutElastic(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}
				// Bounce
				{
			case 27:
				data[num].r = PP::Easing::EaseInBounce(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInBounce(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInBounce(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 28:
				data[num].r = PP::Easing::EaseOutBounce(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseOutBounce(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseOutBounce(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
			case 29:
				data[num].r = PP::Easing::EaseInOutBounce(data[num].r1, data[num].r2, data[num].eTimer3, total);
				data[num].g = PP::Easing::EaseInOutBounce(data[num].g1, data[num].g2, data[num].eTimer3, total);
				data[num].b = PP::Easing::EaseInOutBounce(data[num].b1, data[num].b2, data[num].eTimer3, total);
				data[num].alpha = PP::Easing::EaseInSine(data[num].alpha1, data[num].alpha2, data[num].eTimer3, total);
				break;
				}

			}
		}

		void LibEfc::Update(float elapsedTime)
		{
			// posで!イージング
			if (!save.at(0).easingF[0])
			{
				param->p1 = save.at(0).centerPos[0] + PP::System::GetRandF(-save.at(0).centerRandomPos[0], save.at(0).centerRandomPos[0]); // 生成初期位置
				param->p2 = save.at(0).centerPos[1] + PP::System::GetRandF(-save.at(0).centerRandomPos[1], save.at(0).centerRandomPos[1]); // 生成初期位置
				param->p3 = save.at(0).centerPos[2] + PP::System::GetRandF(-save.at(0).centerRandomPos[2], save.at(0).centerRandomPos[2]); // 生成初期位置
				param->pv1 = save.at(0).velocityPos[0] + PP::System::GetRandF(-save.at(0).velocityRandomPos[0], save.at(0).velocityRandomPos[0]); // 速度
				param->pv2 = save.at(0).velocityPos[1] + PP::System::GetRandF(-save.at(0).velocityRandomPos[1], save.at(0).velocityRandomPos[1]); // 速度
				param->pv3 = save.at(0).velocityPos[2] + PP::System::GetRandF(-save.at(0).velocityRandomPos[2], save.at(0).velocityRandomPos[2]); // 速度
				param->pa1 = save.at(0).accelPos[0] + PP::System::GetRandF(-save.at(0).accelRandomPos[0], save.at(0).accelRandomPos[0]); // 加速度
				param->pa2 = save.at(0).accelPos[1] + PP::System::GetRandF(-save.at(0).accelRandomPos[1], save.at(0).accelRandomPos[1]); // 加速度
				param->pa3 = save.at(0).accelPos[2] + PP::System::GetRandF(-save.at(0).accelRandomPos[2], save.at(0).accelRandomPos[2]); // 加速度
			}
			// sizeで!イージング
			if (!save.at(0).easingF[1])
			{
				param->sizeX = save.at(0).centerScale[0] + PP::System::GetRandF(-save.at(0).centerRandomScale[0], save.at(0).centerRandomScale[0]);
				param->sizeY = save.at(0).centerScale[1] + PP::System::GetRandF(-save.at(0).centerRandomScale[1], save.at(0).centerRandomScale[1]);
			}
			// colorで!イージング
			if (!save.at(0).easingF[2])
			{
				param->r = save.at(0).centerColor[0] + PP::System::GetRandF(-save.at(0).centerRandomColor[0], save.at(0).centerRandomColor[0]); // カラー赤
				param->g = save.at(0).centerColor[1] + PP::System::GetRandF(-save.at(0).centerRandomColor[1], save.at(0).centerRandomColor[1]); // カラー緑
				param->b = save.at(0).centerColor[2] + PP::System::GetRandF(-save.at(0).centerRandomColor[2], save.at(0).centerRandomColor[2]); // カラー青
				param->a = save.at(0).centerColor[3] + PP::System::GetRandF(-save.at(0).centerRandomColor[3], save.at(0).centerRandomColor[3]); // 透明度
			}

			param->existTimer = static_cast<int>(PP::System::GetRandF(static_cast<float>(save.at(0).existTimer[0]), static_cast<float>(save.at(0).existTimer[1]))); // 生存時間

			param->rot += PP::System::GetToRadian(save.at(0).rotation);
			param->p3 += (cosf(param->rot) * save.at(0).radius) * save.at(0).Axis[0]; // X軸回転
			param->p2 += (sinf(param->rot) * save.at(0).radius) * save.at(0).Axis[0]; // X軸回転
			param->p1 += (cosf(param->rot) * save.at(0).radius) * save.at(0).Axis[1]; // Y軸回転
			param->p3 += (sinf(param->rot) * save.at(0).radius) * save.at(0).Axis[1]; // Y軸回転
			param->p2 += (cosf(param->rot) * save.at(0).radius) * save.at(0).Axis[2]; // Z軸回転
			param->p1 += (sinf(param->rot) * save.at(0).radius) * save.at(0).Axis[2]; // Z軸回転

			// posでイージング
			if (save.at(0).easingF[0])
			{
				param->esp1 = save.at(0).easingStartPos[0] + PP::System::GetRandF(-save.at(0).easingRandomStartPos[0], save.at(0).easingRandomStartPos[0]);
				param->esp2 = save.at(0).easingStartPos[1] + PP::System::GetRandF(-save.at(0).easingRandomStartPos[1], save.at(0).easingRandomStartPos[1]);
				param->esp3 = save.at(0).easingStartPos[2] + PP::System::GetRandF(-save.at(0).easingRandomStartPos[2], save.at(0).easingRandomStartPos[2]);
				param->eep1 = save.at(0).easingEndPos[0] + PP::System::GetRandF(-save.at(0).easingRandomEndPos[0], save.at(0).easingRandomEndPos[0]);
				param->eep2 = save.at(0).easingEndPos[1] + PP::System::GetRandF(-save.at(0).easingRandomEndPos[1], save.at(0).easingRandomEndPos[1]);
				param->eep3 = save.at(0).easingEndPos[2] + PP::System::GetRandF(-save.at(0).easingRandomEndPos[2], save.at(0).easingRandomEndPos[2]);
			}
			// sizeでイージング
			if (save.at(0).easingF[1])
			{
				param->size1X = save.at(0).easingStartScale[0] + PP::System::GetRandF(-save.at(0).easingRandomStartScale[0], save.at(0).easingRandomStartScale[0]);
				param->size1Y = save.at(0).easingStartScale[1] + PP::System::GetRandF(-save.at(0).easingRandomStartScale[1], save.at(0).easingRandomStartScale[1]);
				param->size2X = save.at(0).easingEndScale[0] + PP::System::GetRandF(-save.at(0).easingRandomEndScale[0], save.at(0).easingRandomEndScale[0]);
				param->size2Y = save.at(0).easingEndScale[1] + PP::System::GetRandF(-save.at(0).easingRandomEndScale[1], save.at(0).easingRandomEndScale[1]);
			}
			// colorでイージング
			if (save.at(0).easingF[2])
			{
				param->r1 = save.at(0).easingStartColor[0] + PP::System::GetRandF(-save.at(0).easingRandomStartColor[0], save.at(0).easingRandomStartColor[0]);
				param->g1 = save.at(0).easingStartColor[1] + PP::System::GetRandF(-save.at(0).easingRandomStartColor[1], save.at(0).easingRandomStartColor[1]);
				param->b1 = save.at(0).easingStartColor[2] + PP::System::GetRandF(-save.at(0).easingRandomStartColor[2], save.at(0).easingRandomStartColor[2]);
				param->a1 = save.at(0).easingStartColor[3] + PP::System::GetRandF(-save.at(0).easingRandomStartColor[3], save.at(0).easingRandomStartColor[3]);
				param->r2 = save.at(0).easingEndColor[0] + PP::System::GetRandF(-save.at(0).easingRandomEndColor[0], save.at(0).easingRandomEndColor[0]);
				param->g2 = save.at(0).easingEndColor[1] + PP::System::GetRandF(-save.at(0).easingRandomEndColor[1], save.at(0).easingRandomEndColor[1]);
				param->b2 = save.at(0).easingEndColor[2] + PP::System::GetRandF(-save.at(0).easingRandomEndColor[2], save.at(0).easingRandomEndColor[2]);
				param->a2 = save.at(0).easingEndColor[3] + PP::System::GetRandF(-save.at(0).easingRandomEndColor[3], save.at(0).easingRandomEndColor[3]);
			}

			float time = elapsedTime;
			for (int i = 0; i < num; i++)
			{
				if (ReleaseFlag) data[i].type = -1;
				if (data[i].type < 0) continue;

				if (save.at(0).easingF[0])
				{
					data[i].eTimer1 += time;
					if (data[i].eTimer1 > save.at(0).totalTime[0]) data[i].eTimer1 = 0;
					EaseSetPosition(save.at(0).index[0], i, save.at(0).totalTime[0]);
				}
				if (save.at(0).easingF[1])
				{
					data[i].eTimer2 += time;
					if (data[i].eTimer2 > save.at(0).totalTime[1]) data[i].eTimer2 = 0;
					EaseSetScale(save.at(0).index[1], i, save.at(0).totalTime[1]);
				}
				if (save.at(0).easingF[2])
				{
					data[i].eTimer3 += time;
					if (data[i].eTimer3 > save.at(0).totalTime[2]) data[i].eTimer3 = 0;
					EaseSetColor(save.at(0).index[2], i, save.at(0).totalTime[2]);
				}

				if (!save.at(0).easingF[0])
				{
					data[i].pvx += data[i].pax * time;
					data[i].pvy += data[i].pay * time;
					data[i].pvz += data[i].paz * time;
					data[i].px += data[i].pvx * time;
					data[i].py += data[i].pvy * time;
					data[i].pz += data[i].pvz * time;
				}

				data[i].angle += data[i].spin;
				data[i].timer--;
				if (data[i].timer <= 0) data[i].type = -1;
			}

			// 再生
			if (playFlag) addTimer++;
			if (addTimer < save.at(0).makeTimer)
			{
				if (playFlag) Set();
			}
			else
			{
				playFlag = false;
				addTimer = 0;
			}
			// 消去
			ReleaseFlag = false;
		}

		void LibEfc::Render(ID3D11DeviceContext* dc, const RenderContext& rc)
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
				vertex[n].Position.x = data[i].px + uP.x; // 仮置き
				vertex[n].Position.y = data[i].py + uP.y; // 仮置き
				vertex[n].Position.z = data[i].pz + uP.z; // 仮置き
				vertex[n].Tex.x = data[i].w;
				vertex[n].Tex.y = data[i].h;
				vertex[n].Normal.x = data[i].angle;
				vertex[n].Color = { data[i].r, data[i].g, data[i].b, data[i].alpha };
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

		void LibEfc::Set()
		{
			for (int i = 0; i < num; i++)
			{
				if (data[i].type >= 0) continue;
				// 座標関連
				data[i].type = 0;
				if (!save.at(0).easingF[0])
				{
					data[i].px = param->p1 + iP.x;
					data[i].py = param->p2 + iP.y;
					data[i].pz = param->p3 + iP.z;
					data[i].pvx = param->pv1;
					data[i].pvy = param->pv2;
					data[i].pvz = param->pv3;
					data[i].pax = param->pa1;
					data[i].pay = param->pa2;
					data[i].paz = param->pa3;
				}
				if (save.at(0).easingF[0])
				{
					data[i].px1 = param->esp1 + iP.x; // 始点
					data[i].py1 = param->esp2 + iP.y; // 始点
					data[i].pz1 = param->esp3 + iP.z; // 始点
					data[i].px2 = param->eep1 + iP.x; // 終点
					data[i].py2 = param->eep2 + iP.y; // 終点
					data[i].pz2 = param->eep3 + iP.z; // 終点
				}
				// 拡縮関連
				if (!save.at(0).easingF[1])
				{
					data[i].w = param->sizeX;
					data[i].h = param->sizeY;
				}
				if (save.at(0).easingF[1])
				{
					data[i].w1 = param->size1X;
					data[i].h1 = param->size1Y;
					data[i].w2 = param->size2X;
					data[i].h2 = param->size2Y;
				}
				// 色関連
				if (!save.at(0).easingF[2])
				{
					data[i].r = param->r;
					data[i].g = param->g;
					data[i].b = param->b;
					data[i].alpha = param->a;
				}
				if (save.at(0).easingF[2])
				{
					data[i].r1 = param->r1;
					data[i].g1 = param->g1;
					data[i].b1 = param->b1;
					data[i].alpha1 = param->a1;
					data[i].r2 = param->r2;
					data[i].g2 = param->g2;
					data[i].b2 = param->b2;
					data[i].alpha2 = param->a2;
				}
				// 回転関連
				data[i].angle = save.at(0).angle;
				data[i].spin = save.at(0).spin;
				// 時間関連
				data[i].timer = static_cast<float>(param->existTimer);
				if (save.at(0).easingF[0]) data[i].eTimer1 = 0;
				if (save.at(0).easingF[1]) data[i].eTimer2 = 0;
				if (save.at(0).easingF[2]) data[i].eTimer3 = 0;
				break;
			}
		}

		void LibEfc::Play(DirectX::XMFLOAT3 pos)
		{
			playFlag = true;
			iP = pos;
		}

		void LibEfc::Release()
		{
			ReleaseFlag = true;
			playFlag = false;
			addTimer = 0;
		}

		void LibEfc::SetPosition(DirectX::XMFLOAT3 pos)
		{
			uP = pos;
		}
	}
}