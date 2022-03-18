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

			void LoadTexture(const char* fileName); // LibEfc�ɗp����Texture�̓ǂݍ���

			void Set();
			void SetPosition(DirectX::XMFLOAT3 pos);

			void Play(DirectX::XMFLOAT3 pos = { 0, 0, 0 });

			void Release();

			struct Save
			{
				// pos
				float centerPos[3] = {}; // ���W(���S)
				float centerRandomPos[3] = {}; // ���W(�U��)
				float velocityPos[3] = {}; // ���x(���S)
				float velocityRandomPos[3] = {}; // ���x(�U��)
				float accelPos[3] = {}; // �����x(���S)
				float accelRandomPos[3] = {}; // �����x(�U��)
				float easingStartPos[3] = {}; // �C�[�W���O�n�_(���S)
				float easingRandomStartPos[3] = {}; // �C�[�W���O�n�_(�U��)
				float easingEndPos[3] = {}; // �C�[�W���O�I�_(���S)
				float easingRandomEndPos[3] = {}; // �C�[�W���O�I�_(�U��)

				// angle
				float radius = 0; // ���a
				float rotation = 0; // ����]
				float angle = 0; // �������p�x
				float spin = 0; // ��]�p�x

				// scale
				float centerScale[2] = { 1.0f, 1.0f }; // �g�k(���S)
				float centerRandomScale[2] = {}; // �g�k(�U��)
				float easingStartScale[2] = { 1.0f, 1.0f }; // �C�[�W���O�n�_(���S)
				float easingRandomStartScale[2] = {}; // �C�[�W���O�n�_(�U��)
				float easingEndScale[2] = { 1.0f, 1.0f }; // �C�[�W���O�I�_(���S)
				float easingRandomEndScale[2] = {}; // �C�[�W���O�I�_(�U��)

				// color
				float centerColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };// �F(���S)
				float centerRandomColor[4] = {}; // �F(�U��)
				float easingStartColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // �C�[�W���O�n�_(���S)
				float easingRandomStartColor[4] = {}; // �C�[�W���O�n�_(�U��)
				float easingEndColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // �C�[�W���O�I�_(���S)
				float easingRandomEndColor[4] = {}; // �C�[�W���O�I�_(�U��)

				float totalTime[3] = { 5.0f, 5.0f, 5.0f };
				int existTimer[2] = { 60, 60 }; // ��������(�ŏ�&�ő�)
				int makeTimer = 2; // ��������
				int index[3] = {}; // �C�[�W���O�摜�ԍ�
				char fileName[128] = {}; // �e�N�X�`���̃p�X
				bool Axis[3] = {}; // ��]��
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
				float px = 0, py = 0, pz = 0; // ���W
				float px1 = 0, py1 = 0, pz1 = 0; // �n�_
				float px2 = 0, py2 = 0, pz2 = 0; // �I�_
				float pvx = 0, pvy = 0, pvz = 0; // ���x
				float pax = 0, pay = 0, paz = 0; // �����x

				float w = 0, h = 0; // ���@����
				float w1 = 0, h1 = 0;
				float w2 = 0, h2 = 0;

				float r = 0, g = 0, b = 0;
				float alpha = 0;
				float r1 = 0, g1 = 0, b1 = 0;
				float alpha1 = 0;
				float r2 = 0, g2 = 0, b2 = 0;
				float alpha2 = 0;

				float timer = 0;
				float angle = 0; // �A���O��
				float spin = 0;
				float ax = 0, ay = 0, az = 0;
				int type = 0;

				float eTimer1 = 0;
				float eTimer2 = 0;
				float eTimer3 = 0;

			};

			struct Param // �X�V����l
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

			int num = 0; // ���q�̐������� * 3
			int addTimer = 0;
			bool playFlag = false;
			bool ReleaseFlag = false;

			DirectX::XMFLOAT3 iP = {}; // �����ʒu
			DirectX::XMFLOAT3 uP = {}; // �X�V�ʒu

			// �萔�o�b�t�@�̃f�[�^��`
			struct ConstantBufferForPerFrame
			{
				DirectX::XMFLOAT4X4	view;			// �r���[�ϊ��s��
				DirectX::XMFLOAT4X4	projection;		// �����ϊ��s��
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