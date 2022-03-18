#include "Pfc.hlsli"



float4 main(PS_OUT pin) : SV_TARGET0
{
    float4 color = DiffuseTexture.Sample(DecalSampler, pin.Tex) * pin.Color;
    return color;
}