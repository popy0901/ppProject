#include "DepthShadow.hlsli"


//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(VS_OUT pin) : SV_TARGET0
{
	float depth = pin.Depth.z / pin.Depth.w;
	return float4(depth,(depth * depth), 0, 1);
}