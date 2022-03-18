//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//���_�J���[����

	float3 wNormal : TEXCOORD1; //���[���h�@��
	float3 wPosition : TEXCOORD2; //���[���h���W

	float4 Depth : TEXCOORD3; // �[�x�l
};

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
};
