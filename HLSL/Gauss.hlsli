//--------------------------------------------
//	テクスチャ
//--------------------------------------------

Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------

/*struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};*/

struct VS_OUT
{
	float4 Position : SV_POSITION;
	float4 Color    : COLOR;
	float2 Texcoord : TEXCOORD;
};


// グローバル変数
#define BUFFER_SIZE 256
cbuffer CBPerFrame2 : register(b3)
{
	float4 Weight[BUFFER_SIZE];
	float KarnelSize;
	float2 Texcel;
	float dummy;
};