#include "Pfc.hlsli"




GS_OUT main(
    float3 position : POSITION,
    float3 normal : NORMAL,
    float2 texcoord : TEXCOORD,
    float4 color : COLOR)
{
    GS_OUT gout = (GS_OUT) 0;
    gout.Position = position;
    gout.Normal = normal;
    gout.Tex = texcoord;
    gout.Color = color;
    
    return gout;

}
