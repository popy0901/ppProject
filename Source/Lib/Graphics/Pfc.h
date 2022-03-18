#pragma once

#include <wrl.h>
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "Lib/Graphics/RenderContext.h"

namespace PP
{
	namespace Class
	{
		class LibEfc
		{
		public:

			LibEfc(const char* fileName);
			~LibEfc()
			{
				data.clear();
				save.clear();
				delete param;
				delete[] vertex;
			}

			void Update(float elapsedTime = (1.0f / 60.0f));

			void Render(ID3D11DeviceContext* dc, const RenderContext& rc);

			void LoadTexture(const char* fileName); // LibEfcに用いるTextureの読み込み

			void Set();
			void SetPosition(DirectX::XMFLOAT3 pos);

			void Play(DirectX::XMFLOAT3 pos = { 0, 0, 0 });

			void Release();

			struct Save
			{
				// pos
				float centerPos[3] = {}; // 座標(中心)
				float centerRandomPos[3] = {}; // 座標(振幅)
				float velocityPos[3] = {}; // 速度(中心)
				float velocityRandomPos[3] = {}; // 速度(振幅)
				float accelPos[3] = {}; // 加速度(中心)
				float accelRandomPos[3] = {}; // 加速度(振幅)
				float easingStartPos[3] = {}; // イージング始点(中心)
				float easingRandomStartPos[3] = {}; // イージング始点(振幅)
				float easingEndPos[3] = {}; // イージング終点(中心)
				float easingRandomEndPos[3] = {}; // イージング終点(振幅)

				// angle
				float radius = 0; // 半径
				float rotation = 0; // 軸回転
				float angle = 0; // 発生時角度
				float spin = 0; // 回転角度

				// scale
				float centerScale[2] = { 1.0f, 1.0f }; // 拡縮(中心)
				float centerRandomScale[2] = {}; // 拡縮(振幅)
				float easingStartScale[2] = { 1.0f, 1.0f }; // イージング始点(中心)
				float easingRandomStartScale[2] = {}; // イージング始点(振幅)
				float easingEndScale[2] = { 1.0f, 1.0f }; // イージング終点(中心)
				float easingRandomEndScale[2] = {}; // イージング終点(振幅)

				// color
				float centerColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };// 色(中心)
				float centerRandomColor[4] = {}; // 色(振幅)
				float easingStartColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // イージング始点(中心)
				float easingRandomStartColor[4] = {}; // イージング始点(振幅)
				float easingEndColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // イージング終点(中心)
				float easingRandomEndColor[4] = {}; // イージング終点(振幅)

				float totalTime[3] = { 5.0f, 5.0f, 5.0f };
				int existTimer[2] = { 60, 60 }; // 生存時間(最小&最大)
				int makeTimer = 2; // 生成時間
				int index[3] = {}; // イージング画像番号
				char fileName[128] = {}; // テクスチャのパス
				bool Axis[3] = {}; // 回転軸
				bool easingF[3] = {}; // 0 = pos, 1 = sacle, 2 = color;
				bool dummy[2] = {};

				template<class Archive>
				void serialize(Archive& archive, int version);
			};

			std::vector<Save>& GetSave() { return save; }

		private:

			void Create();

			void EaseSetPosition(int index, int num, float total);
			void EaseSetScale(int index, int num, float total);
			void EaseSetColor(int index, int num, float total);

			struct Data
			{
				float px = 0, py = 0, pz = 0; // 座標
				float px1 = 0, py1 = 0, pz1 = 0; // 始点
				float px2 = 0, py2 = 0, pz2 = 0; // 終点
				float pvx = 0, pvy = 0, pvz = 0; // 速度
				float pax = 0, pay = 0, paz = 0; // 加速度

				float w = 0, h = 0; // 幅　高さ
				float w1 = 0, h1 = 0;
				float w2 = 0, h2 = 0;

				float r = 0, g = 0, b = 0;
				float alpha = 0;
				float r1 = 0, g1 = 0, b1 = 0;
				float alpha1 = 0;
				float r2 = 0, g2 = 0, b2 = 0;
				float alpha2 = 0;

				float timer = 0;
				float angle = 0; // アングル
				float spin = 0;
				float ax = 0, ay = 0, az = 0;
				int type = 0;

				float eTimer1 = 0;
				float eTimer2 = 0;
				float eTimer3 = 0;

			};

			struct Param // 更新する値
			{
				float p1 = 0, p2 = 0, p3 = 0;
				float pv1 = 0, pv2 = 0, pv3 = 0;
				float pa1 = 0, pa2 = 0, pa3 = 0;
				float esp1 = 0, esp2 = 0, esp3 = 0;
				float eep1 = 0, eep2 = 0, eep3 = 0;

				float sizeX = 0, sizeY = 0;
				float size1X = 0, size1Y = 0;
				float size2X = 0, size2Y = 0;

				float r = 0, g = 0, b = 0, a = 0;
				float r1 = 0, g1 = 0, b1 = 0, a1 = 0;
				float r2 = 0, g2 = 0, b2 = 0, a2 = 0;

				float rot = 0;

				int existTimer = 0;
			};

			struct Vertex
			{
				DirectX::XMFLOAT3	Position;
				DirectX::XMFLOAT3	Normal; // angle, 
				DirectX::XMFLOAT2	Tex;
				DirectX::XMFLOAT4	Color;
			};

			std::vector<Save> save;
			std::vector<Data> data;
			Param* param = nullptr;
			Vertex* vertex = nullptr;

			int num = 0; // 粒子の生成時間 * 3
			int addTimer = 0;
			bool playFlag = false;
			bool ReleaseFlag = false;

			DirectX::XMFLOAT3 iP = {}; // 発生位置
			DirectX::XMFLOAT3 uP = {}; // 更新位置

			// 定数バッファのデータ定義
			struct ConstantBufferForPerFrame
			{
				DirectX::XMFLOAT4X4	view;			// ビュー変換行列
				DirectX::XMFLOAT4X4	projection;		// 透視変換行列
			};

			Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				constantBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>		geometryShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState[2] = {};

			Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView = nullptr;
		};
	}
}