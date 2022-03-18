#pragma once

#include "Lib/Lib.h"

class Object
{
public:

	Object() {}
	virtual ~Object() {}

	// �ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { wc.position = position; }

	// �ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return wc.position; }

	// ��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { wc.angle = angle; }

	// ��]�擾
	const DirectX::XMFLOAT3& GetAngle() const { return wc.angle; }

	// �g�k�ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { wc.scale = scale; }

	// �g�k�擾
	const DirectX::XMFLOAT3& GetScale() const { return wc.scale; }

	// ���a�ݒ�
	void SetRadius(const float& radius) { this->radius = radius; }

	// ���a�擾
	float GetRadius() const { return radius; }

	// �����ݒ�
	void SetHeight(const float& height) { this->height = height; }

	// �����擾
	float GetHeight() const { return height; }

	// �̗͐ݒ�
	void SetHealth(const int& health) { this->health = health; }

	// �̗͎擾
	int GetHealth() const { return health; }

	// �ő�̗͐ݒ�
	void SetMaxHealth(const int& maxHealth) { this->maxHealth = maxHealth; }

	// �ő�̗͎擾
	int GetMaxHealth() const { return maxHealth; }

	// �n�ʂɐڂ��Ă��邩
	bool IsGround() const { return isGround; }

	// �_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime);

	// �Ռ���^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

protected:

	// �ړ�
	void Move(float vx, float vz, float speed);

	// ����
	void Turn(float vx, float vz, float speed, float elapsedTime);

	// �W�����v
	void Jump(float speed);

	// ���͍X�V����
	void UpdateVelocity(float elapsedTime);

	// ���G���ԍX�V����
	void UpdateInvincibleTimer(float elapsedTime);

	// ���n��
	virtual void OnLanding() {}

	// �K�[�h��(����v���C���[�̂�)
	virtual void OnGuarded() {};

	// �_���[�W��
	virtual void OnDamaged() {}

	// ���S��
	virtual void OnDead() {}

private:

	// �������͍X�V����
	void UpdateVerticalVelocity(float elapsedFrame);

	// �����ړ��X�V����
	void UpdateVerticalMove(float elapsedTime);

	// �������͍X�V����
	void UpdateHorizontalVelocity(float elapsedFrame);

	// �����ړ��X�V����
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