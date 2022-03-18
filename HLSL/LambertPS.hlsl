
#include "Lambert.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	//return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
    
    // NoLight
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * float4(1, 1, 1, 1);
}

