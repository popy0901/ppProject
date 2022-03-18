//--------------------------------------------
//	テクスチャ
//--------------------------------------------


Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//頂点カラー無し

	float3 wNormal : TEXCOORD1; //ワールド法線
	float3 wPosition : TEXCOORD2; //ワールド座標

	float4 Depth : TEXCOORD3; // 深度値
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
