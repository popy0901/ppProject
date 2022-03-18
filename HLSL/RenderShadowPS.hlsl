#include "RenderShadow.hlsli"

//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------
float4 main(VS_OUT pin) : SV_TARGET0
{
	float4 color = DiffuseTexture.Sample(DecalSampler,pin.Tex);
	float3 N = normalize(pin.wNormal);
	float3 E = normalize(EyePos.xyz - pin.wPosition);
	float3 L = normalize(lightDirection.xyz);
	// 環境光
	float3 A = AmbientColor.rgb;

	//拡散反射
	float3 C = LightColor.rgb;
	float3 Kd = float3(1.0f, 1.0f, 1.0f);
	float3 D = Diffuse(N, L, C, Kd);

	//鏡面反射
	float3 Ks = float3(1.0f, 1.0f, 1.0f);
	float3 S = PhongSpecular(N, L, C, E, Ks, 20);

	color *= pin.Color * float4(A + D + S, 1.0);

	// シャドウマップ適用
	color.rgb *= GetShadow(ShadowTexture, ShadowSampler,
		pin.vShadow, ShadowColor, Bias);

	return color;
}


