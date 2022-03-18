#include "Destraction.hlsli"



GS_OUT main(
	float4 position     : POSITION,
	float3 normal : NORMAL,
	float3 tangent : TANGENT,
	float2 texcoord : TEXCOORD,
	float4 color : COLOR,
	float4 boneWeights : WEIGHTS,
	uint4  boneIndices : BONES)
{
	GS_OUT gout = (GS_OUT)0;
	gout.Position = position;
	gout.Normal = normal;
	gout.Tex = texcoord;
	gout.Color = color;
		
	gout.BoneWeights = boneWeights;
	gout.BoneIndices = boneIndices;

	return gout;
}