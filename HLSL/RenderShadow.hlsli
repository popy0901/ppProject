//--------------------------------------------
//	�e�N�X�`��
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);

Texture2D ShadowTexture : register(t3);
SamplerState ShadowSampler : register(s3);

//--------------------------------------------
//	�f�[�^�[�t�H�[�}�b�g
//--------------------------------------------
struct VS_OUT
{
	float4 Position : SV_POSITION;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOR;	//���_�J���[����

	float3 wNormal : TEXCOORD1; //���[���h�@��
	float3 wPosition : TEXCOORD2; //���[���h���W
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

	float4	LightColor;		//���C�g�̐F
	float4	AmbientColor;	//����
	float4  EyePos;			//�J�������W

	float4x4 LightViewProjection;
	float4	ShadowColor;
};

static const float Bias = 0.0008;

//���[���h���W�����C�g��ԍ��W�ɕϊ�
float3 GetShadowTex(float4x4 vp, float3 wPos)
{
	// ���K���f�o�C�X���W�n
	float4 wvpPos;
	wvpPos.xyz = wPos;
	wvpPos.w = 1;
	wvpPos = mul(vp, wvpPos);
	wvpPos /= wvpPos.w;
	// �e�N�X�`�����W�n
	wvpPos.y = -wvpPos.y;
	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
	//wvpPos.x = (1 + wvpPos.x) * 0.5f;
	//wvpPos.y = (1 - wvpPos.y) * 0.5f;
	return wvpPos.xyz;
}

static const float BIAS = 0.000001f;
//�V���h�[�}�b�v����V���h�[��ԍ��W�ɕϊ���Z�l��r
float3 GetShadow(Texture2D st, SamplerState ss, float3 Tex, float3 Scolor,
	float Bias)
{ 
	float2 d = st.Sample(ss, Tex.xy).rg;
	float R = (d.x * d.x);
	float G = d.y;
	// ���U�̌v�Z
	float variance = max(BIAS, G - R);
	// �m����̍ő�l�̎Z�o
	float p_max = saturate(variance / (variance + ((Tex.z - d.x) * (Tex.z - d.x))));
	// �e�̐F
	Scolor = lerp(Scolor, 1.0f, p_max);
	// �Б��s�������L���Ȃ̂� Tex.z > d.x �̏ꍇ����
	return max(Scolor, p_max);
}

//--------------------------------------------
//	�g�U���ˊ֐�(�����o�[�g)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�

float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = max(0, D);			// ���̒l���O�ɂ���
	return C * D * K;
}

//--------------------------------------------
//	�n�[�t�����o�[�g
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// K:���˗�
float3 HalfLambert(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, -L);
	D = D * 0.5 + 0.5;
	//D = D * D;	// ���������Ɍ�������
	return C * D * K;
}

//--------------------------------------------
//	���ʔ��ˊ֐�(�u�����E�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 BlinnPhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//�n�[�t�x�N�g��
	float3 H = normalize(-L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}
//--------------------------------------------
//	���ʔ��ˊ֐�(�t�H��)
//--------------------------------------------
// N:�@��(���K���ς�)
// L:���˃x�N�g��(���K���ς�)
// C:���ˌ�(�F�E����)
// E:���_�����x�N�g��(���K���ς�)
// K:���˗�(0�`1.0)
// Power:�n�C���C�g�̋���(�P���x)

float3 PhongSpecular(float3 N, float3 L, float3 C, float3 E,
	float3 K, float Power)
{
	//���˃x�N�g��
	float3 R = normalize(reflect(L, N));
	float3 S = dot(R, E);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}