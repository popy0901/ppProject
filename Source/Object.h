#pragma once

#include "Lib/Lib.h"

class Object
{
public:

	Object() {}
	virtual ~Object() {}

	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { wc.position = position; }

	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return wc.position; }

	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { wc.angle = angle; }

	// 回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return wc.angle; }

	// 拡縮設定
	void SetScale(const DirectX::XMFLOAT3& scale) { wc.scale = scale; }

	// 拡縮取得
	const DirectX::XMFLOAT3& GetScale() const { return wc.scale; }

	// 半径設定
	void SetRadius(const float& radius) { this->radius = radius; }

	// 半径取得
	float GetRadius() const { return radius; }

	// 高さ設定
	void SetHeight(const float& height) { this->height = height; }

	// 高さ取得
	float GetHeight() const { return height; }

	// 体力設定
	void SetHealth(const int& health) { this->health = health; }

	// 体力取得
	int GetHealth() const { return health; }

	// 最大体力設定
	void SetMaxHealth(const int& maxHealth) { this->maxHealth = maxHealth; }

	// 最大体力取得
	int GetMaxHealth() const { return maxHealth; }

	// 地面に接しているか
	bool IsGround() const { return isGround; }

	// ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);

	// 衝撃を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

protected:

	// 移動
	void Move(float vx, float vz, float speed);

	// 旋回
	void Turn(float vx, float vz, float speed, float elapsedTime);

	// ジャンプ
	void Jump(float speed);

	// 速力更新処理
	void UpdateVelocity(float elapsedTime);

	// 無敵時間更新処理
	void UpdateInvincibleTimer(float elapsedTime);

	// 着地時
	virtual void OnLanding() {}

	// ガード時(現状プレイヤーのみ)
	virtual void OnGuarded() {};

	// ダメージ時
	virtual void OnDamaged() {}

	// 死亡時
	virtual void OnDead() {}

private:

	// 垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedFrame);

	// 垂直移動更新処理
	void UpdateVerticalMove(float elapsedTime);

	// 水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedFrame);

	// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

protected:

	PP::WorldContext wc = {};
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };

	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float moveSpeed;
	float radius = 0.5f;
	float height = 1.0f;
	float invincibleTimer = 1.0f;
	float friction = 0.5f;
	float gravity = -1.0f;
	float accel = 1.0f;
	float maxSpeed = 5.0f;
	float airControl = 0.3f;
	float stepOffset = 1.0f;
	float slopeRate = 1.0f;

	int health = 3;
	int maxHealth = 3;

	bool isGround = false;

};