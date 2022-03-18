#include "Gauss.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
/*PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	// 出力値設定.
	output.Position = float4(input.Position, 1);
	output.Color = input.Color;
	output.Tex = input.Tex;

	return output;
}*/

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
	vout.Position = position;
	vout.Color = color;
	vout.Texcoord = texcoord;

	return vout;
}