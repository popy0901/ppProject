#include "Destraction.hlsli"



// 回転行列の作成
matrix rotation(float3 angle)
{
	float sx, sy, sz, cx, cy, cz;
	sincos(angle.x, sx, cx);
	sincos(angle.y, sy, cy);
	sincos(angle.z, sz, cz);
	matrix rot;
	rot._11 = cy * cz;
	rot._12 = cy * sz;
	rot._13 = -sy;
	rot._21 = sx * sy * cz - cx * sz;
	rot._22 = sx * sy * sz + cx * cz;
	rot._23 = sx * cy;
	rot._31 = cx * sy * cz + sx * sz;
	rot._32 = cx * sy * sz - sx * cz;
	rot._33 = cx * cy;
	rot._14 = rot._24 = rot._34 = rot._41 = rot._42 = rot._43 = 0;
	rot._44 = 1;
	return rot;
}

[maxvertexcount(3)]
void main(triangle GS_OUT input[3], inout TriangleStream<PS_OUT> output)
{
	// ポリゴンの重心を算出
	float3 cPos = (input[0].Position.xyz +
		input[1].Position.xyz +
		input[2].Position.xyz) / 3;

	// 面法線を算出
	float3 AB = input[1].Position - input[0].Position;
	float3 BC = input[2].Position - input[1].Position;
	float3 N = cross(AB, BC);
	N = normalize(N);

	// 面を作成
	PS_OUT element;
	for (int i = 0; i < 3; ++i)
	{
		float3 P = input[i].Position.xyz;

		// 中心点を起点に拡縮処理
		float3 v = P - cPos;
		v *= Paramter.y;

		// 中心点を起点に回転処理
		v = mul(rotation(Paramter.z), float4(v, 0)).xyz;

		// 面法線方向に位置を変化
		P = v + cPos + N * Paramter.w;

		float3 p = { 0, 0, 0 };
		float3 n = { 0, 0, 0 };
		for (int j = 0; j < 4; j++)
		{
			p += (input[i].BoneWeights[j] * mul(float4(P, 1.0f), boneTransforms[input[i].BoneIndices[j]])).xyz;
			n += (input[i].BoneWeights[j] * mul(float4(input[i].Normal.xyz, 0), boneTransforms[input[i].BoneIndices[j]])).xyz;
		}

		float3 N = normalize(n);
		float3 L = normalize(-lightDirection.xyz);
		float d = dot(L, N);
		float power = max(0, d) * 0.5f + 0.5f;
		element.Position = mul(float4(p, 1.0f), viewProjection);
		element.wNormal = n;
		element.wPosition = p;
		element.Tex = input[i].Tex;
		element.Color.xyz = input[i].Color.xyz * materialColor.xyz * power;
		element.Color.w = input[i].Color.w * materialColor.w;
		output.Append(element); // 頂点生成
	}
	// 追加した頂点で新しいプリミティブを生成する
	output.RestartStrip();
}