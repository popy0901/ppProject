#pragma once

#include <DirectXMath.h>

class Camera
{
public:

	Camera();
	~Camera();

	void Update(float elapsedTime);

	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	void SetHeightOffset(const float heightOffset) { this->heightOffset = heightOffset; }

	// Tps�J����
	void OnTpsMode(DirectX::XMFLOAT3 target);

	// ���b�N�I���J����
	void OnLockonMode(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 target);

	// ���[�V�����J����
	void OnEventMode(void* data);

	// �J�����h��
	void OnShake(float timer, float power); // �h�ꎞ��, �h���

	void DebugRender();

	static Camera& Instance()
	{
		return *instance;
	}

	// ���[�h
	enum class Mode
	{
		TpsCamera_,
		LockonCamera_,
		MotionCamera_,
	};

private:

	// Tps�J�����X�V����
	void TpsCamera(float elapsedTime);

	// ���b�N�I���J�����X�V����
	void LockonCamera(float elapsedTime);

	// ���[�V�����J�����X�V����
	void EventCamera(float elapedTime);

	// �����̃Y�������Z�o
	float CalcSide(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2);

private:

	static Camera *instance;

	Mode mode = Mode::TpsCamera_;

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 target = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };

	float rollSpeed = DirectX::XMConvertToRadians(120);
	float range = 5.0f;

	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-45);

	// ���b�N�I�������p�ϐ�
	DirectX::XMFLOAT3 newPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3 newTarget = { 0, 0, 0 };
	DirectX::XMFLOAT3 targetWork[2] = { {0, 0, 0}, {0, 0, 0} };// 0 : ���W, 1 : �����_
	float lengthLimit[2] = { 3, 5 };
	float sideValue = 1;
	float heightOffset = 0.0f; // �����̕␳

	// ���[�V�����J�����p�ϐ�
	float motionTimer = 0.0f;
	//std::vector<CameraMotionData> motionDatas;

	// �J�����h��p�ϐ�
	float shakeTimer = 0.0f;
	float shakePower = 1.0f;

	// ���b�Z�[�W�L�[

};