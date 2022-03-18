#include "Destraction.hlsli"



float4 main(PS_OUT pin) : SV_TARGET0
{
	float4 color = DiffuseTexture.Sample(DecalSampler,pin.Tex);
	float3 N = normalize(pin.wNormal);
	float3 E = normalize(EyePos.xyz - pin.wPosition);
	float3 L = normalize(lightDirection.xyz);
	// ŠÂ‹«Œõ
	float3 A = AmbientColor.rgb;

	//ŠgŽU”½ŽË
	float3 C = LightColor.rgb;
	float3 Kd = float3(1.0f, 1.0f, 1.0f);
	float3 D = Diffuse(N, L, C, Kd);

	//‹¾–Ê”½ŽË
	float3 Ks = float3(1.0f, 1.0f, 1.0f);
	float3 S = PhongSpecular(N, L, C, E, Ks, 20);

	color *= pin.Color * float4(A + D + S, 1.0);

	return color;
}