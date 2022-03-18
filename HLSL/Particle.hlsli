Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

struct GS_OUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float4 Color : COLOR;
};

struct PS_OUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR0;
    float2 Tex : TEXCOORD0;
};

cbuffer CBPerFrame : register(b0)
{
    matrix View; // ビュー変換行列
    matrix Projection; // 透視変換行列
};