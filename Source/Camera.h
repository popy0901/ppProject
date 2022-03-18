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

	// Tpsカメラ
	void OnTpsMode(DirectX::XMFLOAT3 target);

	// ロックオンカメラ
	void OnLockonMode(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 target);

	// モーションカメラ
	void OnEventMode(void* data);

	// カメラ揺れ
	void OnShake(float timer, float power); // 揺れ時間, 揺れ力

	void DebugRender();

	static Camera& Instance()
	{
		return *instance;
	}

	// モード
	enum class Mode
	{
		TpsCamera_,
		LockonCamera_,
		MotionCamera_,
	};

private:

	// Tpsカメラ更新処理
	void TpsCamera(float elapsedTime);

	// ロックオンカメラ更新処理
	void LockonCamera(float elapsedTime);

	// モーションカメラ更新処理
	void EventCamera(float elapedTime);

	// 横軸のズレ向き算出
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

	// ロックオン処理用変数
	DirectX::XMFLOAT3 newPosition = { 0, 0, 0 };
	DirectX::XMFLOAT3 newTarget = { 0, 0, 0 };
	DirectX::XMFLOAT3 targetWork[2] = { {0, 0, 0}, {0, 0, 0} };// 0 : 座標, 1 : 注視点
	float lengthLimit[2] = { 3, 5 };
	float sideValue = 1;
	float heightOffset = 0.0f; // 高さの補正

	// モーションカメラ用変数
	float motionTimer = 0.0f;
	//std::vector<CameraMotionData> motionDatas;

	// カメラ揺れ用変数
	float shakeTimer = 0.0f;
	float shakePower = 1.0f;

	// メッセージキー

};