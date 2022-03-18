#pragma once

#include "Stage.h"

// 移動床
class StageMoveFloor : public Stage
{
public:

	StageMoveFloor();
	~StageMoveFloor() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void RenderSub() override;

	void DebugRender() override;

	// レイキャスト
	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		PP::HitResult& hit) override;

	// スタート位置設定
	void SetStartPoint(const DirectX::XMFLOAT3& start) { this->start = start; }

	// ゴール位置設定
	void SetGoalPoint(const DirectX::XMFLOAT3& goal) { this->goal = goal; }

	// トルク設定
	void SetTorque(const DirectX::XMFLOAT3& torque) { this->torque = torque; }

private:

	void UpdateTransform();

private:

	PP::ppModel model = nullptr;

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { 0, 0, 0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };

	DirectX::XMFLOAT4X4 transform =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	DirectX::XMFLOAT4X4 oldTransform =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	DirectX::XMFLOAT3 torque = { 0, 0, 0 };
	DirectX::XMFLOAT3 start = { 0, 0, 0 };
	DirectX::XMFLOAT3 goal = { 0, 0, 0 };

	DirectX::XMFLOAT3 oldAngle = { 0, 0, 0 };

	float moveSpeed = 2.0f;
	float moveRate = 0.0f;

};