#include "Player.h"
#include "SceneManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Shot.h"
#include "EffectManager.h"
#include "Navigation.h"

Player::Player()
{
	PP::Model::Make(model, "PLAYER");
	//efc = new Effect01();
	//EffectManager::Instance().Register(efc);

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetPosition({ 0.0f, 0.0f, 0.5f });
	SetRadius(0.5f);
	SetHeight(1.4f);
	SetHealth(5);
	SetMaxHealth(GetHealth());
	stamina = 50;
	maxStamina = stamina;
	PP::Model::UpdateTransform(model, wc);
	TransIdleState(); // �����̏�Ԑݒ�
}

Player::~Player()
{
	//delete model;
}

void Player::Update(float elapsedTime)
{
	UpdateCameraState(elapsedTime);

	PP::Model::UpdateAnimation(model, elapsedTime);

	PP::Model::UpdateSubAnimation(model, elapsedTime);

	switch (state)
	{
	case State::IDLE:
		UpdateIdleState(elapsedTime);
		break;
	case State::WALK:
		UpdateWalkState(elapsedTime);
		break;
	case State::RUN:
		UpdateRunState(elapsedTime);
		break;
	case State::DASH:
		UpdateDashState(elapsedTime);
		break;
	case State::JUMP:
		//UpdateJumpState(elapsedTime);
		break;
	case State::FALL:
		UpdateFallState(elapsedTime);
		break;
	case State::LAND:
		UpdateLandState(elapsedTime);
		break;
	case State::ATTACK:
		AttackMoveUpdate(elapsedTime);
		break;
	case State::DODGE:
		UpdateDodgeState(elapsedTime);
		break;
	case State::DAMAGE:
		UpdateDamageState(elapsedTime);
		break;
	case State::GUARD_IDLE:
		UpdateGuardIdleState(elapsedTime);
		break;
	case State::GUARD_WALK:
		UpdateGuardWalkState(elapsedTime);
		break;
	case State::GUARD_RUN:
		UpdateGuardRunState(elapsedTime);
		break;
	case State::GUARD_DASH:
		UpdateGuardDashState(elapsedTime);
		break;
	case State::GUARD_REACTION:
		UpdateGuardReactionState(elapsedTime);
		break;
	case State::DEATH:
		UpdateDeathState(elapsedTime);
		break;
	}

	// ���u�� // ��قǊe��Ԋ֐����ŌĂяo��?
	if (FallCheck() && state != State::DODGE)
		TransFallState();

	//if (PP::Input::CheckHitKey('Q') == 1) // �������m�F�p����
	//{
	//	velocity.y = 0;
	//	wc.position.y += 15;
	//}

	UpdateHeal();

	UpdateStamina();

	UpdateVelocity(elapsedTime);

	UpdateInvincibleTimer(elapsedTime);

	CollisionPlayerVsEnemies();
	
	PP::Model::UpdateTransform(model, wc);
}

void Player::Render()
{
	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void Player::RenderSub()
{
	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void Player::DebugRender()
{
	//if (invincibleTimer <= 0) // ���G���Ԃ��Ȃ��ꍇ�\��
	//	PP::Debug::Primitive3D::DrawCylinder(wc.position, radius * 30, height, { 0, 0, 0, 1 });

	// ���C�̕`��
	/*{
		float my = velocity.y * PP::System::GetElapsedTime();

		DirectX::XMFLOAT3 start = { wc.position.x , wc.position.y + stepOffset, wc.position.z };
		DirectX::XMFLOAT3 end = { wc.position.x, wc.position.y + my + gravity, wc.position.z };

		float mx = velocity.x * PP::System::GetElapsedTime();
		float mz = velocity.z * PP::System::GetElapsedTime();

		DirectX::XMFLOAT3 start2 = { wc.position.x, wc.position.y + stepOffset, wc.position.z };
		DirectX::XMFLOAT3 end2 = { wc.position.x + mx, wc.position.y + stepOffset, wc.position.z + mz };

		PP::Debug::Primitive3D::DrawLine(start, end, { 1, 1, 0, 1 });

		PP::Debug::Primitive3D::DrawLine(start2, end2, { 1, 1, 0, 1 });
	}*/

	// �O���x�N�g��
	{
		DirectX::XMFLOAT3 foward = PP::Model::GetForward(model);
		DirectX::XMFLOAT3 v = { wc.position.x + foward.x * 30, wc.position.y + foward.y * 30, wc.position.z + foward.z * 30 };
		PP::Debug::Primitive3D::DrawLine(wc.position, v, { 1, 1, 0, 1 });
	}

	Navigation::Instance().DebugRender();

	if (attackCollisionFlag) // �U�����肪���鎞�ɔ���͈͂̕\��
		PP::Debug::Primitive3D::DrawSphere(weaponPosition, nodeRadius, { 1, 0, 0, 1 });

	PP::Imgui::Begin(u8"PlayerDebug", { 0, 0 }, { 128, 256 });
	PP::Imgui::Text("Position\n x:%2f\n y:%2f\n z:%2f", wc.position.x, wc.position.y, wc.position.z);
	PP::Imgui::Text("Velocity\n x:%2f\n y:%2f\n z:%2f", velocity.x, velocity.y, velocity.z);
	PP::Imgui::Text("fallTimer : %f", fallTimer);
	PP::Imgui::Text("healItem : %d", healItem);
	switch (state)
	{
	case State::IDLE:			PP::Imgui::Text(u8"Idle");				break;
	case State::WALK:			PP::Imgui::Text(u8"Walk");				break;
	case State::RUN:			PP::Imgui::Text(u8"Run");				break;
	case State::DASH:			PP::Imgui::Text(u8"DASH");				break;
	case State::JUMP:			PP::Imgui::Text(u8"JUMP");				break;
	case State::FALL:			PP::Imgui::Text(u8"FALL");				break;
	case State::LAND:			PP::Imgui::Text(u8"LAND");				break;
	case State::ATTACK:			PP::Imgui::Text(u8"ATTACK");			break;
	case State::DODGE:			PP::Imgui::Text(u8"DODGE");				break;
	case State::DAMAGE:			PP::Imgui::Text(u8"DAMAGE");			break;
	case State::GUARD_IDLE:		PP::Imgui::Text(u8"GUARD_IDLE");		break;
	case State::GUARD_WALK:		PP::Imgui::Text(u8"GUARD_WALK");		break;
	case State::GUARD_RUN:		PP::Imgui::Text(u8"GUARD_RUN");			break;
	case State::GUARD_DASH:		PP::Imgui::Text(u8"GUARD_DASH");		break;
	case State::GUARD_REACTION:	PP::Imgui::Text(u8"GUARD_REACTION");	break;
	case State::DEATH:			PP::Imgui::Text(u8"DEATH");				break;
	}
	PP::Imgui::End();
}

bool Player::CheckGuard(int damage, float invincibleTime)
{
	if (!guardFlag) return false; // �K�[�h���Ă��Ȃ��ꍇ��false;

	if (invincibleTimer > 0.0f) return false; // ���G���̂���false

	invincibleTimer = invincibleTime;

	// �K�[�h���̃X�^�~�i����Ɖ񕜂��n�߂�܂ł̎���
	UseStamina(damage, 7.0f); // �X�^�~�i����

	OnGuarded(); // �K�[�h�ʒm

	return true;
}

void Player::OnGuarded()
{
	// �K�[�h���A�N�V�����X�e�[�g�֑J��
	TransGuardReactionState();
}

void Player::OnDamaged()
{
	// �_���[�W�X�e�[�g�֑J��
	TransDamageState();
}

void Player::OnDead()
{
	// ���S�X�e�[�g�֑J��
	TransDeathState();
}

// ���n�������ɌĂ΂��
void Player::OnLanding()
{
	const float fallHeight = 5.0f;
	// ������(�������x�������͈�莞�Ԉȏ�̗����Ŏ��S�ɂ���H)
	if (fallTimer > fallHeight) TransDeath2State();

	fallTimer = 0.0f;

	// ��e���A���S���A������łȂ���Β��n�\
	if (state != State::DAMAGE && state != State::DEATH && state != State::DODGE)
	{
		TransLandState();
	}
}

// ���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	float ax = PP::Input::GetLeftStickX();
	float ay = PP::Input::GetLeftStickY();

	// �J�����̌����ƃX�e�B�b�N���͂Ői�s�������Z�o
	const DirectX::XMFLOAT3& cameraRight = PP::Camera::GetRight();
	const DirectX::XMFLOAT3& cameraFront = PP::Camera::GetFront();

	// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf((cameraRightX * cameraRightX) + (cameraRightZ * cameraRightZ));
	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf((cameraFrontX * cameraFrontX) + (cameraFrontZ * cameraFrontZ));
	if (cameraFrontLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	// �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.y = 0.0f;
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);

	return vec;
}

void Player::UpdateHeal()
{
	if (InputHeal()) // Healing()�I�Ȋ֐��ɂ܂Ƃ߂�
	{
		int health = GetHealth();
		int maxHealth = GetMaxHealth();
		health += maxHealth * 0.5f;
		healItem--;
		if (health >= maxHealth)
			health = maxHealth;
		SetHealth(health);
	}
}

void Player::UpdateStamina() // �X�^�~�i���R��
{
	if (stamina >= maxStamina) // �ő�Ȃ�Ԃ�
	{
		stamina = maxStamina; 
		return;
	}

	if (stamina <= 0) stamina = 0.0f;

	if (state == State::ATTACK || state == State::DODGE) return;

	staminaTimer--;

	if (staminaTimer > 0) return; // �񕜂��n�߂�܂ł̎��Ԃ�����ꍇ�͕Ԃ�

	// �X�^�~�i�ɉ񕜑��x�����Z
	if (guardFlag) stamina += staminaHeal / 2;
	else stamina += staminaHeal;
}

void Player::UseStamina(float use, float timer)
{
	stamina -= use;
	staminaTimer = timer;
}

bool Player::InputMove(float elapsedTime)
{
	DirectX::XMFLOAT3 moveVec = GetMoveVec();

	Move(moveVec.x, moveVec.z, speed);

	Turn(moveVec.x, moveVec.z, turnSpeed, elapsedTime);

	if (lockonFlag) // ���b�N�I�����ł��_�b�V���͈ړ��x�N�g���̕���������
	{
		// �v���C���[�̍��W���烍�b�N�I����̓G
		const DirectX::XMFLOAT2 lockVec = { 
			lockonEnemy->GetPosition().x - wc.position.x,
			lockonEnemy->GetPosition().z - wc.position.z };

		const float turnSpeed2 = turnSpeed * 2.0f;
		const float turnSpeed10 = turnSpeed * 10.0f;
		// ���b�N�I���Ώۂ̕�������
		if (state != State::DASH)
		{
			// ��������G�̕����։�]
			Turn(lockVec.x, lockVec.y, turnSpeed10, elapsedTime);
		}
		else 
			Turn(moveVec.x, moveVec.z, turnSpeed2, elapsedTime);
	}

	// �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	return moveVec.x != 0.0f || moveVec.y != 0.0f || moveVec.z != 0.0f;
}

bool Player::InputWalk()
{
	float ax = PP::Input::GetLeftStickX();
	float ay = PP::Input::GetLeftStickY();

	float l = sqrtf(ax * ax + ay * ay);
	const float walkRange = 0.8f; // ���s���̍ő����

	if (l < walkRange)
	{
		return true;
	}
	return false;
}

bool Player::InputDash()
{
	if (stamina <= 0) return false; // �X�^�~�i���Ȃ����false

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButton() & GamePad::BTN_RIGHT_SHOULDER)
	{
		return true;
	}
	return false;
}

bool Player::InputJump()
{
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	/*if (pad.GetButtonDown() & GamePad::BTN_A)
	{
			Jump(jumpSpeed);
			return true; // ���͂�����
	}*/
	return false;
}

bool Player::InputAttack()
{
	if (stamina <= 0) return false; // �X�^�~�i���Ȃ����false

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_X)
	{
		return true;
	}
	return false;
}

bool Player::InputGuard() // �p��
{
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButton() & GamePad::BTN_LEFT_SHOULDER || PP::Input::CheckHitKey('B'))
	{
		return true;
	}
	return false;
}

bool Player::InputHeal()
{
	if (healItem <= 0) return false;
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_Y) // ���L�[
	{
		return true; // ���͂�����
	}

	return false;
}

bool Player::InputDodge() 
{
	if (stamina <= 0) return false; // �X�^�~�i���Ȃ����false

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_B)
	{
		return true;
	}
	return false;
}

bool Player::FallCheck()
{
	// �n�ʂɂ��Ȃ������ւ̉����x������
	return !IsGround() && (velocity.y) < 0.0f;
}

void Player::InputProjectile()
{
	 // ���Ԃ����Ȃ�
}

void Player::UpdateCameraState(float elapsedTime)
{
	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	bool oldLockonFlag = lockonFlag;
	Enemy* oldLockonEnemy = lockonEnemy;

	lockonFlag = false;
	lockonEnemy = nullptr;

	switch (state)
	{
	case State::IDLE:
	case State::WALK:
	case State::RUN:
	case State::DASH:
	case State::JUMP:
	case State::FALL:
	case State::LAND:
	case State::ATTACK:
	case State::DODGE:
	case State::DAMAGE:
	case State::GUARD_IDLE:
	case State::GUARD_WALK:
	case State::GUARD_RUN:
	case State::GUARD_DASH:
	case State::GUARD_REACTION:
	case State::DEATH:
		// ���b�N�I���t���O�؂�ւ�
		if (pad.GetButtonDown() & GamePad::BTN_RIGHT_THUMB || PP::Input::CheckHitKey('R') == 1)
		{
			lockonButtonFlag ^= true;
		}
		// ���b�N�I�����[�h
		if (lockonButtonFlag)
		{
			DirectX::XMVECTOR p, t, v;
			if (!oldLockonFlag) // �O�񃍃b�N�I�����ĂȂ��Ȃ�
			{
				// (�v���C���[������͈͓���)��ԋ߂��G������
				float length1, length2;
				EnemyManager& enemyManager = EnemyManager::Instance();
				for (int i = 0; i < enemyManager.GetEnemyCount(); ++i)
				{
					Enemy* enemy = enemyManager.GetEnemy(i);

					if (lockonFlag) // ���񃍃b�N�I�����Ă���
					{
						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&lockonEnemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p); // �^�[�Q�b�g - �v���C���[
						DirectX::XMStoreFloat(&length2, DirectX::XMVector2LengthSq(v));

						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&enemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p); // �^�[�Q�b�g - �v���C���[
						DirectX::XMStoreFloat(&length1, DirectX::XMVector2LengthSq(v));

						if (length1 < length2)
						{
							lockonEnemy = enemy;
							DirectX::XMStoreFloat3(&lockonDirection, DirectX::XMVector3Normalize(v));
						}
					}
					else // ���񃍃b�N�I�����Ă��Ȃ�
					{
						// ���͈͊O�Ȃ�Ԃ�(Tps�J�����ɖ߂�)
						if (!(PP::Collision::SphereVsSphere(wc.position, (GetRadius() * 30),
							enemy->GetPosition(), enemy->GetRadius(), DirectX::XMFLOAT3(0, 0, 0))))
						{
							lockonButtonFlag = false;
							break;
						}
						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&enemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p);
						DirectX::XMStoreFloat(&length1, DirectX::XMVector3LengthSq(v));

						lockonEnemy = enemy;
						DirectX::XMStoreFloat3(&lockonDirection, DirectX::XMVector3Normalize(v));
						lockonFlag = true;
					}
				}
				//break;
			}
			else // �O�񃍃b�N�I�����Ă���
			{
				// ���b�N�I�����Ă��G�̑��݂�(�v���C���[������͈͓�)����Όp��
				EnemyManager& enemyManager = EnemyManager::Instance();
				for (int i = 0; i < enemyManager.GetEnemyCount(); ++i)
				{
					Enemy* enemy = enemyManager.GetEnemy(i);

					// ���͈͊O�Ȃ�Ԃ�(Tps�J�����ɖ߂�)
					if (!(PP::Collision::SphereVsSphere(wc.position, (GetRadius() * 30),
						enemy->GetPosition(), enemy->GetRadius(), DirectX::XMFLOAT3(0, 0, 0))))
					{
						lockonButtonFlag = false;
						break;
					}

					if (enemy == oldLockonEnemy)
					{
						lockonEnemy = enemy;
						lockonFlag = true;
						p = DirectX::XMLoadFloat3(&wc.position);
						t = DirectX::XMLoadFloat3(&enemy->GetPosition());
						v = DirectX::XMVectorSubtract(t, p);

						//lockonEnemy = enemy;
						DirectX::XMStoreFloat3(&lockonDirection, DirectX::XMVector3Normalize(v));
						break;
					}
					// ���b�N�I�����Ă����G�����Ȃ��ꍇ��Tps�J�����ɖ߂�
					else lockonButtonFlag = false;
				}
				// ���b�N�I���Ώۂ̐؂�ւ�(�E�X�e�B�b�NX)
				float ax = PP::Input::GetRightStickX();
				lockonTargetChangeTime -= elapsedTime * 60.0f;
				if (lockonEnemy && // ���b�N�I���Ώۂ�����
					lockonTargetChangeTime <= 0 && // �؂�ւ��\��
					ax * ax > 0.3f) // �X�e�B�b�N��|���Ă���
				{
					lockonTargetChangeTime = lockonTargetChangeTimeMax;
					// ���b�N�I���ΏۂƎ����̈ʒu����x�N�g�����Z�o
					float dx = oldLockonEnemy->GetPosition().x - wc.position.x;
					float dz = oldLockonEnemy->GetPosition().z - wc.position.z;
					float l = sqrtf(dx * dx + dz * dz);
					dx /= l;
					dz /= l;
					// �O�ς�p���č��E������s���A�p�x�I�ɂ��ł��߂��Ώۂ����b�N�I��
					float angleMax = FLT_MAX;
					for (int i = 0; i < enemyManager.GetEnemyCount(); ++i)
					{
						Enemy* enemy = enemyManager.GetEnemy(i);
						if (enemy == oldLockonEnemy) continue;
						float dx2 = enemy->GetPosition().x - wc.position.x;
						float dz2 = enemy->GetPosition().z - wc.position.z;
						float l2 = sqrtf(dx2 * dx2 + dz2 * dz2);
						dx2 /= l2;
						dz2 /= l2;
						float cross = dx * dz2 - dz * dx2;
						if (ax > 0 && cross < 0)
						{
							cross = abs(cross);
							if (cross < angleMax)
							{
								angleMax = cross;
								lockonEnemy = enemy;
							}
						}
						else if (ax < 0 && cross > 0)
						{
							if (cross < angleMax)
							{
								angleMax = cross;
								lockonEnemy = enemy;
							}
						}
					}
				}
				//break;
			}
			if (lockonFlag)
			{
				// �J���������b�N�I�����[�h�ɂ���
				lockonEnemy->SetLockedFlag(true);
				DirectX::XMFLOAT3 enmPos = lockonEnemy->GetPosition();
				float heightOffset = lockonEnemy->GetHeight() * 0.5f;
				Camera::Instance().OnLockonMode(wc.position, enmPos);
				Camera::Instance().SetHeightOffset(heightOffset);
				break;
			}
		}
		// �J������ʏ탂�[�h(TPS)�ɂ���
		Camera::Instance().OnTpsMode(wc.position);
		break;
	}
}

void Player::LockonMoveAnimation()
{
	// ���b�N�I�����̎a�㍶�E�ړ��ɂ��A�j���[�V�����ݒ�
	if (lockonFlag)
	{
		DirectX::XMFLOAT3 v = { lockonEnemy->GetPosition().x - wc.position.x,
		0,
		lockonEnemy->GetPosition().z - wc.position.z };
		DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&v); // �v���C���[����G�l�~�[�ւ̃x�N�g��
		DirectX::XMVECTOR M = DirectX::XMLoadFloat3(&(GetMoveVec())); // �v���C���[�̈ړ��x�N�g��
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(V), DirectX::XMVector3Normalize(M));
		DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(DirectX::XMVector3Normalize(V), DirectX::XMVector3Normalize(M));
		float dot = 0.0f;
		DirectX::XMFLOAT3 cross = {};
		DirectX::XMStoreFloat(&dot, Dot);
		DirectX::XMStoreFloat3(&cross, Cross); // c.y = 1 or c.y = -1
		dot = acosf(dot); // 0 �` 180
		float radian = 0.0f;
		radian = DirectX::XMConvertToRadians(45.0f);

		if (cross.y < 0) dot = (radian * 8) - dot;

		// �E
		if ((dot > (radian * 1) && dot <= (radian * 3)) && runNum != 1) // �E
		{
			runNum = 1;
			TransRunState();
		}
		// ��
		if ((dot > (radian * 3) && dot <= (radian * 5)) && runNum != 2) // ��
		{
			runNum = 2;
			TransRunState();
		}
		// ��
		if ((dot > (radian * 5) && dot <= (radian * 7)) && runNum != 3) // ��
		{
			runNum = 3;
			TransRunState();
		}
		// �O
		if ((dot > (radian * 7) || dot <= (radian * 1)) && runNum != 4) // ��
		{
			runNum = 4;
			TransRunState();
		}
	}
	// �񃍃b�N�I�����͑O
	if (!lockonFlag && runNum != 4)
	{
		runNum = 4;
		TransRunState();
	}
}

// �ҋ@
void Player::TransIdleState()
{
	state = State::IDLE;

	idleTimer = 600;

	PP::Model::PlayAnimation(model, ANIM_IDLE1, true);
}

void Player::UpdateIdleState(float elapsedTime)
{
	// ���͏���(Walk, Jump, Attack1, Dodge)
	{
		if (InputMove(elapsedTime))
		{
			TransWalkState(); // ���s�X�e�[�g�֑J��
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack())
		{
			TransAttack1State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}

		if (InputGuard())
		{
			TransGuardIdleState();
		}
	}

	// InputProjectile();

	if (guardFlag) idleTimer = 600;
	idleTimer--;

	if (idleTimer == 0) // ��莞�ԓ����Ȃ��ꍇ�ʂ̑ҋ@�A�j���[�V�������Đ�
	{
		PP::Model::PlayAnimation(model, PP::System::GetRandF(static_cast<float>(ANIM_IDLE2), static_cast<float>(ANIM_HIT1)), false);
	}

	if (!PP::Model::IsPlayAnimation(model)) // �ʂ̃A�j���[�V�����Đ����I���������ǂ�
		TransIdleState();
}

// ���s
void Player::TransWalkState()
{
	state = State::WALK;

	speed = 2.0f;

	// �O��ԂŃK�[�h���Ă����ꍇ�A�j���[�V�����Đ����Ȃ�
	if (!guardedFlag)
		PP::Model::PlayAnimation(model, ANIM_WALK, true);
	else guardedFlag = false;
}

void Player::UpdateWalkState(float elapsedTime)
{
	// ���͏���(Idle, Run, Jump, Attack1, Dodge)
	{
		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // �ҋ@�X�e�[�g�֑J��
		}

		if (!InputWalk())
		{
			TransRunState(); // ���s�X�e�[�g�֑J��
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack())
		{
			TransAttack1State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}

		if (InputGuard())
		{
			TransGuardWalkState();
		}
	}

	// InputProjectile();
}

// ���s
void Player::TransRunState()
{
	state = State::RUN;

	speed = 5.0f;

	// �O��ԂŃK�[�h���Ă����ꍇ�A�j���[�V�����Đ����Ȃ�
	if (!guardedFlag)
	{
		// �Đ�������s���[�V�����̐ݒ�
		switch (runNum)
		{
		case 1: // �E
			//guardOffset = { 20.0f, -60.0f, -50.0f };
			PP::Model::PlayAnimation(model, ANIM_RIGHT_RUN, true);
			break;
		case 2: // ��
			PP::Model::PlayAnimation(model, ANIM_BACK_RUN, true);
			break;
		case 3: // ��
			PP::Model::PlayAnimation(model, ANIM_LFET_RUN, true);
			break;
		case 4: // �O
			PP::Model::PlayAnimation(model, ANIM_RUN1, true);
			break;
		}
	}
	else guardedFlag = false;
		
	//PP::Model::PlayAnimation(model, ANIM_RUN1, true);
}

void Player::UpdateRunState(float elapsedTime)
{
	// ���͏���(Idle, Walk, Dash, Attack1, Dodge)
	{
		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // �ҋ@�X�e�[�g�֑J��
		}

		if (InputWalk())
		{
			TransWalkState(); // ���s�X�e�[�g�֑J��
		}

		if (InputDash())
		{
			TransDashState(); // ���s(����)�X�e�[�g�֑J��
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack())
		{
			TransAttack1State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}

		if (InputGuard())
		{
			TransGuardRunState();
		}
	}

	LockonMoveAnimation(); // ���b�N�I���ɂ��A�j���[�V�����Đ�

	// InputProjectile();

}

// ���s(��)
void Player::TransDashState()
{
	state = State::DASH;

	speed = 8.0f;

	// �O��ԂŃK�[�h���Ă����ꍇ�A�j���[�V�����Đ����Ȃ�
	if (!guardedFlag)
		PP::Model::PlayAnimation(model, ANIM_RUN2, true);
	else guardedFlag = false;
}

void Player::UpdateDashState(float elapsedTime)
{
	// ���͏���(Idle, Run, Jump, Attack4, Dodge)
	{
		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // �ҋ@�X�e�[�g�֑J��
		}

		if (!InputDash())
		{
			TransRunState(); // ���s�X�e�[�g�֑J��
		}

		/*if (InputJump())
		{
			TransJumpState();
		}*/

		if (InputAttack()) // �����x�ł��݂čU��1���U��4�ւ̑J�ڂ��킯��H
		{
			TransAttack4State();
		}

		if (InputDodge())
		{
			TransDodgeState();
		}
		
		if (InputGuard())
		{
			TransGuardDashState();
		}
	}

	UseStamina(0.1f, 2.0f);

	// InputProjectile();
}

// ����
void Player::TransJumpState()
{
	state = State::JUMP;

	PP::Model::PlayAnimation(model, ANIM_JUMP, false);
}

void Player::UpdateJumpState(float elapsedTime)
{
	InputMove(elapsedTime);

	//if (InputJump()) // 2�i�W�����v�͌�قǂȂ���
	//{
	//	PP::Model::PlayAnimation(model, ANIM_JUMP, false);
	//}

	//if (!PP::Model::IsPlayAnimation(model)) // ��������ύX?(�󒆂ɂ��Ă������xY���}�C�i�X)
	//{
	//	PP::Model::PlayAnimation(model, ANIM_FALL, true); // ������Ԃ�����
	//}

	//InputProjectile();
}

// ����
void Player::TransFallState()
{
	state = State::FALL;

	attackCollisionFlag = false;

	PP::Model::PlayAnimation(model, ANIM_FALL, true);
}

void Player::UpdateFallState(float elapsedTime)
{
	fallTimer += elapsedTime;
	InputMove(elapsedTime);
}

// ���n
void Player::TransLandState()
{
	state = State::LAND;

	PP::Model::PlayAnimation(model, ANIM_LANDING1, false);
}

void Player::UpdateLandState(float elapsedTime)
{
	if (!PP::Model::IsPlayAnimation(model) || InputMove(elapsedTime)) // ���n���[�V����������邩�ړ��ŏ�ԑJ��
	{
		TransIdleState();
	}

	if (InputDodge()) // ���n�L�����Z���p�̉���X�e�[�g�֑J��
	{
		TransDodgeState();
	}
}

// �U���X�V����
void Player::AttackMoveUpdate(float elapsedTime)
{
	if (!PP::Model::IsPlayAnimation(model))
	{
		TransIdleState();
	}

	switch (attackState)
	{
	case 1:
		UpdateAttack1State(elapsedTime);
		break;
	case 2:
		UpdateAttack2State(elapsedTime);
		break;
	case 3:
		UpdateAttack3State(elapsedTime);
		break;
	case 4:
		UpdateAttack4State(elapsedTime);
		break;
	}
}

// �U�����̕␳
void Player::AttackCorrection(const float correctionTimeStart, const float correctionTimeEnd,
	const float collisionTimeStart, const float collisionTimeEnd, 
	const float advanceTime, float elapsedTime) // ���b�N�I�����͓G�̕��Ɍ���
{
	float animationTime = PP::Model::GetCurrentAnimationSeconds(model);
	const char* nodeName = "Sword_joint";
	const DirectX::XMFLOAT3 bladePosition = { 13.0f, -3.0f, 50.0f };

	if (animationTime > advanceTime)
	{
		if (InputAttack())
		{
			cancelAttackFlag = true;
			cancelDodgeFlag = false;
		}
		if (InputDodge())
		{
			cancelDodgeFlag = true;
			cancelAttackFlag = false;
		}
	}

	// ���b�N�I�����Ă��邩�Ői�s�x�N�g���̕ύX
	if (animationTime > correctionTimeStart && animationTime < correctionTimeEnd)
	{
		attackDirection = { GetMoveVec().x, GetMoveVec().z };
		if (attackDirection.x == 0.0f && attackDirection.y == 0.0f) // �����ĂȂ��ꍇ�͑O���x�N�g����ݒ�
			attackDirection = { PP::Model::GetForward(model).x, PP::Model::GetForward(model).z };
		if (lockonFlag)
		{
			attackDirection = { lockonDirection.x, lockonDirection.z };
		}
		DirectX::XMVECTOR AttackDirection = DirectX::XMLoadFloat2(&attackDirection);
		AttackDirection = DirectX::XMVector2Normalize(AttackDirection);
		DirectX::XMStoreFloat2(&attackDirection, AttackDirection);
		const float turnSpeed3 = turnSpeed * 3.0f;
		Move(attackDirection.x, attackDirection.y, speed);
		Turn(attackDirection.x, attackDirection.y, turnSpeed3, elapsedTime);
	}

	attackCollisionFlag = animationTime > collisionTimeStart && animationTime < collisionTimeEnd; // ���蔭������
	if (attackCollisionFlag)
	{
		weaponPosition = NodeToNewPosition(nodeName, bladePosition); // �w��m�[�h����ɔ���p�̍��W��ݒ�
		// �w��m�[�h�ƃG�l�~�[�̏Փˏ���
		CollisionAttackVsEnemies(weaponPosition, nodeRadius);
	}

	if (animationTime > collisionTimeEnd) // ���蔭�����Ԃ𒴂�����L�����Z���\
	{
		if (cancelAttackFlag)
		{
			if (attackState == 2) TransAttack3State(); // 3�i�ڂ֔h��
			if (attackState == 1) TransAttack2State(); // 2�i�ڂ֔h��
		}

		if (cancelDodgeFlag) TransDodgeState(); // ���
	}

}

// �U��1
void Player::TransAttack1State()
{
	state = State::ATTACK;

	attackState = 1;

	cancelAttackFlag = false;

	speed = 10.0f;
	UseStamina(10.0f, 4.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK1, false);
}

void Player::UpdateAttack1State(float elapsedTime)
{
	const float correctionTimeStart = 0.25f; // �␳��t����(�n��)
	const float correctionTimeEnd = 0.45f; // �␳��t����(�I���)
	const float collisionTimeStart = 0.425f; // ���蔭������(�n��)
	const float collisionTimeEnd = 0.525f; // ���蔭������(�I���)
	const float advanceTime = 0.0f; // �Ƃ肠����0

	AttackCorrection(correctionTimeStart, correctionTimeEnd,
		collisionTimeStart, collisionTimeEnd, 
		advanceTime, elapsedTime);
}

// �U��2
void Player::TransAttack2State()
{
	state = State::ATTACK;

	attackState = 2;

	cancelAttackFlag = false;

	speed = 10.0f;
	UseStamina(10.0f, 4.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK2, false);
}

void Player::UpdateAttack2State(float elapsedTime)
{
	const float correctionTimeStart = 0.05f; // �␳��t����(�n��)
	const float correctionTimeEnd = 0.2f; // �␳��t����(�I���)
	const float collisionTimeStart = 0.15f; // ���蔭������(�n��)
	const float collisionTimeEnd = 0.4f; // ���蔭������(�I���)
	const float advanceTime = 0.0f; // �Ƃ肠����0

	AttackCorrection(correctionTimeStart, correctionTimeEnd,
		collisionTimeStart, collisionTimeEnd, 
		advanceTime, elapsedTime);
}

// �U��3
void Player::TransAttack3State()
{
	state = State::ATTACK;

	attackState = 3;

	cancelAttackFlag = false;

	speed = 10.0f;
	UseStamina(10.0f, 7.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK3, false);
}

void Player::UpdateAttack3State(float elapsedTime)
{
	const float correctionTimeStart = 0.075f; // �␳��t����(�n��)
	const float correctionTimeEnd = 0.3f; // �␳��t����(�I���)
	const float collisionTimeStart = 0.25f; // ���蔭������(�n��)
	const float collisionTimeEnd = 0.55f; // ���蔭������(�I���)
	const float advanceTime = 0.0f; // �Ƃ肠����0

	AttackCorrection(correctionTimeStart, correctionTimeEnd,
		collisionTimeStart, collisionTimeEnd,
		advanceTime, elapsedTime);
}

// �U��4(DA)
void Player::TransAttack4State()
{
	state = State::ATTACK;

	attackState = 4;

	cancelAttackFlag = false;

	attackDirection = { PP::Model::GetForward(model).x, PP::Model::GetForward(model).z };
	DirectX::XMVECTOR AttackDirection = DirectX::XMLoadFloat2(&attackDirection);
	AttackDirection = DirectX::XMVector2Normalize(AttackDirection);
	DirectX::XMStoreFloat2(&attackDirection, AttackDirection);
	speed = 7.0f;
	UseStamina(10.0f, 7.0f);

	PP::Model::PlayAnimation(model, ANIM_ATTACK4, false);
}

void Player::UpdateAttack4State(float elapedTime) // DA�͐�s���͂�t���Ă��Ȃ�(���邩���H)
{
	// �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��
	float animationTime = PP::Model::GetCurrentAnimationSeconds(model);
	const float collisionTimeStart = 0.6f; // ���蔭������(�n��)
	const float collisionTimeEnd = 0.8f; // ���蔭������(�I���)
	const float moveTime = 0.65f;
	const char* nodeName = "Sword_joint";
	const DirectX::XMFLOAT3 bladePosition = { 13.0f, -3.0f, 50.0f };

	attackCollisionFlag = animationTime > collisionTimeStart && animationTime < collisionTimeEnd; // ���蔭������
	if (attackCollisionFlag)
	{
		weaponPosition = NodeToNewPosition(nodeName, bladePosition); // �w��m�[�h����ɔ���p�̍��W��ݒ�
		// �w��m�[�h�ƃG�l�~�[�̏Փˏ���
		CollisionAttackVsEnemies(weaponPosition, nodeRadius);
	}

	if (animationTime < moveTime) // ����ł�ԍU�������ɐi�s
	{
		Move(attackDirection.x, attackDirection.y, speed);
	}

	if (animationTime > collisionTimeEnd) // ���蔭�����Ԃ𒴂�����L�����Z���\
	{
		if (InputDodge()) TransDodgeState();
	}

}

void Player::TransDodgeState()
{
	state = State::DODGE;

	attackCollisionFlag = false;

	cancelDodgeFlag = false;

	// ����̐i�s�x�N�g����ݒ�
	dodgeDirection = { GetMoveVec().x, GetMoveVec().z };
	if (dodgeDirection.x == 0.0f && dodgeDirection.y == 0.0f) // �����ĂȂ��ꍇ�͑O���x�N�g����ݒ�
		dodgeDirection = { PP::Model::GetForward(model).x, PP::Model::GetForward(model).z };
	DirectX::XMVECTOR DodgeDirection = DirectX::XMLoadFloat2(&dodgeDirection);
	DodgeDirection = DirectX::XMVector2Normalize(DodgeDirection);
	DirectX::XMStoreFloat2(&dodgeDirection, DodgeDirection);

	invincibleTimer = 0.5f;
	speed = 8.0f;
	UseStamina(10.0f, 10.0f);

	PP::Model::PlayAnimation(model, ANIM_DODGE, false);
}

// ���
void Player::UpdateDodgeState(float elapsedTime)
{
	if (!PP::Model::IsPlayAnimation(model))
	{
		TransIdleState();
	}

	if (InputDodge()) cancelDodgeFlag = true;

	// ���u������()
	float animationTime = PP::Model::GetCurrentAnimationSeconds(model);
	if (animationTime > 0.01f && animationTime < 0.8f)
	{
		Move(dodgeDirection.x, dodgeDirection.y, speed);
	}
	else if (animationTime >= 0.8f && animationTime < 1.3f)
	{
		Move(dodgeDirection.x, dodgeDirection.y, speed / 2);
	}

	if (animationTime > 0.7f) // ����L�����Z�����
	{
		if (cancelDodgeFlag)
		{
			TransDodgeState();
		}
	}

	Turn(dodgeDirection.x, dodgeDirection.y, turnSpeed * 2.0f, elapsedTime);
}

// ��e
void Player::TransDamageState()
{
	state = State::DAMAGE;

	attackCollisionFlag = false;

	// �_���[�W���[�V�����Đ�
	PP::Model::PlayAnimation(model, ANIM_HIT1, false);
}

void Player::UpdateDamageState(float elapsedTime)
{
	// �_���[�W���[�V�������I�������ҋ@�X�e�[�g�֑J��
	if (!PP::Model::IsPlayAnimation(model))
	{
		TransIdleState();
	}
}

// ���S
void Player::TransDeathState()
{
	state = State::DEATH;

	// ���S�A�j���[�V�����Đ�
	PP::Model::PlayAnimation(model, ANIM_DEATH1, false);
}

void Player::TransDeath2State()
{
	state = State::DEATH;

	// ���S�A�j���[�V�����Đ�
	PP::Model::PlayAnimation(model, ANIM_DEATH2, false);
	PP::Model::SetCurrentAnimationSeconds(model, 3.0f);
}

void Player::UpdateDeathState(float elapsedTime)
{
	// ���S���[�V�������I�������OVER�֐ݒ�
	if (!PP::Model::IsPlayAnimation(model))
	{
		SceneManager::Instance()->SetChangeType(SceneManager::OVER);
	}
}

// �K�[�h�ҋ@
void Player::TransGuardIdleState()
{
	state = State::GUARD_IDLE;

	guardOffset = { 10.0f, 0.0f, 0.0f };

	if (!guardFlag) // �O��̏�ԂŃK�[�h�����Ă��Ȃ��Ȃ�
	{
		// �K�[�h���
		guardFlag = true;
		// �㔼�g�����K�[�h���[�V�����Đ�
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardIdleState(float elapsedTime)
{
	// ���͏���
	{
		if (!InputGuard())
		{
			DisGuard(); // �K�[�h����

			TransIdleState(); // ���s�X�e�[�g�֑J��
		}

		if (InputMove(elapsedTime))
		{
			TransWalkState(); // ���s�X�e�[�g�֑J��
		}

		if (InputAttack())
		{
			DisGuard(); // �K�[�h����

			TransAttack1State();
		}

		if (InputDodge())
		{
			DisGuard(); // �K�[�h����

			TransDodgeState();
		}
	}

	UpdateGuard(); // �K�[�h���̍X�V

}

// �K�[�h���s
void Player::TransGuardWalkState()
{
	state = State::GUARD_WALK;

	guardOffset = { 0.0f, 30.0f, 10.0f };

	if (!guardFlag) // �O��̏�ԂŃK�[�h�����Ă��Ȃ��Ȃ�
	{
		// �K�[�h���
		guardFlag = true;
		// �㔼�g�����K�[�h���[�V�����Đ�
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardWalkState(float elapsedTime)
{
	// ���͏���
	{
		if (!InputGuard()) // �K�[�h����
		{
			guardedFlag = true; // �A�j���[�V�����Ǘ��̂��߂����ŗ��Ă�

			DisGuard(); // �K�[�h����

			TransWalkState(); // ���s�X�e�[�g�֑J��
		}

		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // �ҋ@�X�e�[�g�֑J��
		}

		if (!InputWalk())
		{
			TransRunState(); // ���s�X�e�[�g�֑J��
		}

		if (InputAttack())
		{
			DisGuard(); // �K�[�h����

			TransAttack1State();
		}

		if (InputDodge())
		{
			DisGuard(); // �K�[�h����

			TransDodgeState();
		}
	}

	UpdateGuard(); // �K�[�h���̍X�V

}

// �K�[�h���s
void Player::TransGuardRunState()
{
	state = State::GUARD_RUN;

	guardOffset = { 0.0f, 30.0f, 10.0f };

	if (!guardFlag) // �O��̏�ԂŃK�[�h�����Ă��Ȃ��Ȃ�
	{
		// �K�[�h���
		guardFlag = true;
		// �㔼�g�����K�[�h���[�V�����Đ�
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardRunState(float elapsedTime)
{
	// ���͏���
	{
		if (!InputGuard()) // �K�[�h����
		{
			guardedFlag = true; // �A�j���[�V�����Ǘ��̂��߂����ŗ��Ă�

			DisGuard(); // �K�[�h����

			TransRunState(); // ���s�X�e�[�g�֑J��
		}

		//if (/*�K�[�h����*/)
		//{
		//	// GuardHited�X�e�[�g�֑J��
		//}

		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // �ҋ@�X�e�[�g�֑J��
		}

		if (InputWalk())
		{
			TransWalkState(); // ���s�X�e�[�g�֑J��
		}

		if (InputDash())
		{
			TransDashState(); // ���s(����)�X�e�[�g�֑J��
		}

		if (InputAttack())
		{
			DisGuard(); // �K�[�h����

			TransAttack1State();
		}

		if (InputDodge())
		{
			DisGuard(); // �K�[�h����

			TransDodgeState();
		}
	}

	UpdateGuard(); // �K�[�h���̍X�V

	LockonMoveAnimation(); // ���b�N�I���ɂ��A�j���[�V�����Đ�
}

// �K�[�h���s(����)
void Player::TransGuardDashState()
{
	state = State::GUARD_DASH;

	guardOffset = { 0.0f, 30.0f, 10.0f };

	if (!guardFlag) // �O��̏�ԂŃK�[�h�����Ă��Ȃ��Ȃ�
	{
		// �K�[�h���
		guardFlag = true;
		// �㔼�g�����K�[�h���[�V�����Đ�
		PP::Model::PlaySubAnimation(model, "LeftShoulder", ANIM_GUARD2, true);
	}
}

void Player::UpdateGuardDashState(float elapsedTime)
{
	// ���͏���
	{
		if (!InputGuard()) // �K�[�h����
		{
			guardedFlag = true; // �A�j���[�V�����Ǘ��̂��߂����ŗ��Ă�

			DisGuard(); // �K�[�h����

			TransDashState(); // ���s�X�e�[�g�֑J��
		}

		if (!InputMove(elapsedTime))
		{
			TransIdleState(); // �ҋ@�X�e�[�g�֑J��
		}

		if (!InputDash())
		{
			TransRunState(); // ���s�X�e�[�g�֑J��
		}

		if (InputAttack()) // �����x�ł��݂čU��1���U��4�ւ̑J�ڂ��킯��H
		{
			DisGuard(); // �K�[�h����

			TransAttack4State();
		}

		if (InputDodge())
		{
			DisGuard(); // �K�[�h����

			TransDodgeState();
		}
	}

	UpdateGuard(); // �K�[�h���̍X�V

	UseStamina(0.1f, 2.0f);

}

// �K�[�h���A�N�V����
void Player::TransGuardReactionState()
{
	state = State::GUARD_REACTION;

	PP::Model::PlayAnimation(model, ANIM_GUARD3, false);
}

void Player::UpdateGuardReactionState(float elapsedTime)
{
	// ���[�V�������I�������K�[�h�ҋ@�X�e�[�g�֑J��
	if (!PP::Model::IsPlayAnimation(model))
	{
		DisGuard(); // �K�[�h����

		TransIdleState();
	}
}

void Player::UpdateGuard() // �K�[�h���̍X�V����
{
	PP::ppNode n = PP::Model::FindNode(model, "LeftShoulder");
	// �w��m�[�h�̃A���O���𒲐�
	DirectX::XMVECTOR Rot = DirectX::XMLoadFloat4(&n->rotate);
	DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(
		DirectX::XMConvertToRadians(guardOffset.x),
		DirectX::XMConvertToRadians(guardOffset.y),
		-DirectX::XMConvertToRadians(guardOffset.z));
	Rot = DirectX::XMQuaternionMultiply(Rot, Q);
	DirectX::XMStoreFloat4(&n->rotate, Rot);
}

void Player::DisGuard() // �K�[�h����
{
	guardFlag = false;
	PP::Model::StopSubAnimation(model);
}

// �v���C���[�ƃG�l�~�[�̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̓G�Ƒ�������ŏՓ˔���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �Փ�(�v���C���[���������)
		DirectX::XMFLOAT3 outPosition = {};
		if (PP::Collision::CylinderVsCylinder(
			enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
			wc.position, radius, height,
			outPosition))
		{
			this->SetPosition(outPosition);
		}
	}
}

// ���@�e�ƃG�l�~�[�̏Փˏ���
void Player::CollisionProjectileVsEnemies()
{

}

void Player::CollisionAttackVsEnemies(const DirectX::XMFLOAT3& attackPosition, float nodeRadius)
{
	// �w��̍��W�ƑS�Ă̓G�𑍓�����ŏՓˏ���
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		// �Փ�
		DirectX::XMFLOAT3 outPosition = {};
		if (PP::Collision::SphereVsCylinder(
			attackPosition, nodeRadius,
			enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
			outPosition))
		{
			// �G�Ƀ_���[�W(0.25f���G)
			if (enemy->ApplyDamage(1, 0.225f))
			{
				// �U�����ꂽ�t���O
				enemy->SetAttackedFlag(true);

				// ������΂�(���Ԃ�Ȃ��Ȃ�)
				{
					/*const float power = 10.0f;
					const DirectX::XMFLOAT3& e = enemy->GetPosition();
					float vx = e.x - attackPosition.x;
					float vz = e.z - attackPosition.z;
					float lengthXZ = sqrtf((vx * vx) + (vz * vz));
					vx /= lengthXZ;
					vz /= lengthXZ;
					DirectX::XMFLOAT3 impulse = {};
					impulse.x = vx * power;
					impulse.y = power * 0.5f;
					impulse.z = vz * power;*/

					//enemy->AddImpulse(impulse);
				}
			}
		}
	}

}

DirectX::XMFLOAT3 Player::NodeToNewPosition(const char* nodeName, const DirectX::XMFLOAT3& position)
{
	PP::ppNode node = PP::Model::FindNode(model, nodeName);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T; // ���̃��[���h�s����쐬
	DirectX::XMMATRIX BoneWorldTransform = DirectX::XMLoadFloat4x4(&node->worldTransform);

	DirectX::XMFLOAT4X4 w;
	DirectX::XMStoreFloat4x4(&w, (W * BoneWorldTransform));

	return { w._41, w._42, w._43 };
}