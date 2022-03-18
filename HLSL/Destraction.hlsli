//--------------------------------------------
// テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

//--------------------------------------------
// データーフォーマット
//--------------------------------------------

struct GS_OUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR; //頂点カラーあるとき

	float4 BoneWeights : WEIGHTS;
	uint4  BoneIndices : BONES;
};

struct PS_OUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
	float3 wPosition : TEXCOORD0; // ワールド座標
	float3 wNormal : TEXCOORD1; // ワールド法線
	float2 Tex : TEXCOORD2;
};

//--------------------------------------------
// グローバル変数
//--------------------------------------------
cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
	float4				lightDirection;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4				materialColor;

	float4	LightColor;		//ライトの色
	float4	AmbientColor;	//環境光
	float4  EyePos;			//カメラ座標

	float4 Paramter; // x = 破壊レート, y = 拡縮係数, z = 回転係数, w = 移動係数
};

//--------------------------------------------
//	拡散反射関数
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率(0～1.0)

//ランバートシェーディング
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// 負の値を０にする
	return K * C * D;
}

//--------------------------------------------
//	鏡面反射関数(ブリン・フォン)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// E:視点方向ベクトル(正規化済み)
// K:反射率(0～1.0)
// Power:ハイライトの強さ(輝き度)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//ハーフベクトル
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}