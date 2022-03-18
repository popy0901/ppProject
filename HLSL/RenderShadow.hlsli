//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

Texture2D ShadowTexture : register(t3);
SamplerState ShadowSampler : register(s3);

//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//頂点カラー無し

	float3 wNormal : TEXCOORD1; //ワールド法線
	float3 wPosition : TEXCOORD2; //ワールド座標
	float3 vShadow : TEXCOORD4;
};

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

	float4x4 LightViewProjection;
	float4	ShadowColor;
};

static const float Bias = 0.0008;

//ワールド座標をライト空間座標に変換
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// 正規化デバイス座標系
	float4 wvpPos;
	wvpPos.xyz = wPos;
	wvpPos.w = 1;
	wvpPos = mul(vp, wvpPos);
	wvpPos /= wvpPos.w;
	// テクスチャ座標系
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
	//wvpPos.x = (1 + wvpPos.x) * 0.5f;
	//wvpPos.y = (1 - wvpPos.y) * 0.5f;
	return wvpPos.xyz;
}

static const float BIAS = 0.000001f;
//シャドーマップからシャドー空間座標に変換とZ値比較
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor,
	float Bias)
{ 
	float2 d = st.Sample(ss, Tex.xy).rg;
	float R = (d.x * d.x);
	float G = d.y;
	// 分散の計算
	float variance = max(BIAS, G - R);
	// 確率上の最大値の算出
	float p_max = saturate(variance / (variance + ((Tex.z - d.x) * (Tex.z - d.x))));
	// 影の色
	Scolor = lerp(Scolor, 1.0f, p_max);
	// 片側不等式が有効なのは Tex.z > d.x の場合だけ
	return max(Scolor, p_max);
}

//--------------------------------------------
//	拡散反射関数(ランバート)
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// 負の値を０にする
	return C * D * K;
}

//--------------------------------------------
//	ハーフランバート
//--------------------------------------------
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// C:入射光(色・強さ)
// K:反射率
float3 HalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = D * 0.5 + 0.5;
	//D = D * D;	// いい感じに減衰する
	return C * D * K;
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

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
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
//--------------------------------------------
//	鏡面反射関数(フォン)
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
	//反射ベクトル
	float3 R = normalize(reflect(L, N));
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}