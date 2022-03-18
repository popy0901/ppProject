#include "Gauss.hlsli"



/*float4 main(PSInput input) : SV_TARGET0
{
	float4 color = (float4)0;
	color.a = input.Color.a;
	for (int i = 0; i < KarnelSize * KarnelSize; i++)
	{
		float2 offset = texcel * Weight[i].xy;
		float weight = Weight[i].z;
		color.rgb += DiffuseTexture.Sample(DecalSampler, input.Tex + offset).rgb * weight;
	}

	return color;
}*/

float4 main(VS_OUT pin) : SV_TARGET0
{
	float4 color = (float4)0;
	color.w = pin.Color.w;
	for (int i = 0; i < KarnelSize * KarnelSize; i++)
	{
		float2 offset = Texcel * Weight[i].xy;
		float weight = Weight[i].z;
		color.xyz += DiffuseTexture.Sample(DecalSampler, pin.Texcoord + offset).xyz * weight;
	}
	return color;
}