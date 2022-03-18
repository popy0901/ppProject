#include "RenderShadow.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
VS_OUT main(
	float4 position     : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4  boneIndices : BONES
)
{
	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
		n += (boneWeights[i] * mul(float4(normal.xyz, 0), boneTransforms[boneIndices[i]])).xyz;
	}

	VS_OUT vout;
	vout.Position = mul(float4(p, 1.0f), viewProjection);
	vout.wPosition = p;

	float3 N = normalize(n);
	normal = N;
	vout.wNormal = normal;

	float3 L = normalize(-lightDirection.xyz);

	float d = dot(L, N);
	float power = max(0, d) * 0.5f + 0.5f;
	vout.Color.rgb = color.rgb * materialColor.rgb * power * 1.5f; // 1.5fは明るさ調整
	vout.Color.a = color.a * materialColor.a;
	vout.Tex = texcoord;

	vout.vShadow = GetShadowTex(LightViewProjection, vout.wPosition);

	return vout;
}