//--------------------------------------------
// �e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
// �f�[�^�[�t�H�[�}�b�g
//--------------------------------------------

struct GS_OUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR; //���_�J���[����Ƃ�

	float4 BoneWeights : WEIGHTS;
	uint4  BoneIndices : BONES;
};

struct PS_OUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wPosition : TEXCOORD0; // ���[���h���W
	float3 wNormal : TEXCOORD1; // ���[���h�@��
	float2 Tex : TEXCOORD2;
};

//--------------------------------------------
// �O���[�o���ϐ�
//--------------------------------------------
cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4				materialColor;

	float4	LightColor;		//���C�g�̐F
	float4	AmbientColor;	//����
	float4  EyePos;			//�J�������W

	float4 Paramter; // x = �j�󃌁[�g, y = �g�k�W��, z = ��]�W��, w = �ړ��W��
};

//--------------------------------------------
//	�g�U���ˊ֐�
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�(0�`1.0)

//�����o�[�g�V�F�[�f�B���O
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// ���̒l���O�ɂ���
	return K * C * D;
}

//--------------------------------------------
//	���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//�n�[�t�x�N�g��
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}