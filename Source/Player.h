#pragma once

#include "Object.h"
#include "Enemy.h"
#include "Effect.h"

// �v���C���[
class Player : public Object
{
public:

	Player();
	~Player() override;

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	float GetStamina() const { return stamina; }

	float GetMaxStamina() const { return maxStamina; }

	int GetHealItem() const { return healItem; }

	bool CheckGuard(int damage, float invincibleTime);

private:

	// �K�[�h��
	void OnGuarded() override;

	// �_���[�W��
	void OnDamaged() override;

	// ���S��
	void OnDead() override;

	// ���n�������ɌĂ΂��
	void OnLanding() override;

	// ���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;

	void UpdateHeal();

	void UpdateStamina();

	void UseStamina(float use, float timer); // �g�p�ʁA�񕜂܂ł̎���

	bool InputMove(float elapsedTime);

	bool InputWalk();

	bool InputDash();

	bool InputJump();

	bool InputAttack();

	bool InputGuard();

	bool InputHeal();

	bool InputDodge();

	bool FallCheck(); // ��������

	void InputProjectile();

	// �J�����̏�Ԃ��X�V
	void UpdateCameraState(float elapsedTime);

	// ���b�N�I�����̃A�j���[�V����������
	void LockonMoveAnimation(); 

	// �ҋ@�X�e�[�g�֑J��
	void TransIdleState();

	// �ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	// ���s�X�e�[�g�J��
	void TransWalkState();

	// ���s�X�e�[�g�X�V����
	void UpdateWalkState(float elapsedTime);

	// ���s�X�e�[�g�֑J��
	void TransRunState();

	// ���s�X�e�[�g�X�V����
	void UpdateRunState(float elapsedTime);

	// ���s(����)�X�e�[�g�֑J��
	void TransDashState();

	// ���s(����)�X�e�[�g�X�V����
	void UpdateDashState(float elapsedTime);

	// �W�����v�X�e�[�g�֑J��
	void TransJumpState();

	// �W�����v�X�e�[�g�X�V����
	void UpdateJumpState(float elapsedTime);

	// �����X�e�[�g�֑J��
	void TransFallState();

	// �����X�e�[�g�X�V����
	void UpdateFallState(float elapedTime);

	// ���n�X�e�[�g�J��
	void TransLandState();

	// ���n�X�e�[�g�X�V����
	void UpdateLandState(float elapsedTime);

	// �e�U���̍X�V����
	void AttackMoveUpdate(float elapsedTime);

	// �U�����̕␳					�␳��t���Ԃ̊J�n				�␳��t���Ԃ̏I��
	//				���蔭�����Ԃ̊J�n				���蔭�����Ԃ̏I��
	//			��s���͉\����
	void AttackCorrection(const float correctionTimeStart, const float correctionTimeEnd,
		const float collisionTimeStart, const float collisionTimeEnd, 
		const float advanceTime, float elapsedTime);

	// �U��1�X�e�[�g�֑J��
	void TransAttack1State();

	// �U��1�X�e�[�g�X�V����
	void UpdateAttack1State(float elapsedTime);

	// �U��2�X�e�[�g�֑J��
	void TransAttack2State();

	// �U��2�X�e�[�g�X�V����
	void UpdateAttack2State(float elapsedTime);

	// �U��3�X�e�[�g�֑J��
	void TransAttack3State();

	// �U��3�X�e�[�g�X�V����
	void UpdateAttack3State(float elapsedTime);

	// �U��4�X�e�[�g�֑J��
	void TransAttack4State();

	// �U��4�X�e�[�g�X�V����
	void UpdateAttack4State(float elapsedTime);

	// ����X�e�[�g�֑J��
	void TransDodgeState();

	// ����X�e�[�g�X�V����
	void UpdateDodgeState(float elapsedTime);

	// �_���[�W�X�e�[�g�֑J��
	void TransDamageState();

	// �_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	// ���S�X�e�[�g�֑J��
	void TransDeathState();
	void TransDeath2State();

	// ���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);

	// �K�[�h�ҋ@�X�e�[�g�֑J��
	void TransGuardIdleState();

	// �K�[�h�ҋ@�X�e�[�g�X�V����
	void UpdateGuardIdleState(float elapsedTime);

	// �K�[�h���s�X�e�[�g�֑J��
	void TransGuardWalkState();

	// �K�[�h���s�X�e�[�g�X�V����
	void UpdateGuardWalkState(float elapsedTime);

	// �K�[�h���s�X�e�[�g�֑J��
	void TransGuardRunState();

	// �K�[�h���s�X�e�[�g�X�V����
	void UpdateGuardRunState(float elapsedTime);

	// �K�[�h���s(����)�X�e�[�g�֑J��
	void TransGuardDashState();

	// �K�[�h���s(����)�X�e�[�g�X�V����
	void UpdateGuardDashState(float elapsedTime);

	// �K�[�h���A�N�V�����X�e�[�g�֑J��
	void TransGuardReactionState();

	// �K�[�h���A�N�V�����X�V����
	void UpdateGuardReactionState(float elapsedTime);

	void UpdateGuard(); // �K�[�h���̍X�V����
	void DisGuard(); // �K�[�h����

	// �v���C���[�ƃG�l�~�[�̏Փˏ���
	void CollisionPlayerVsEnemies();

	// ���@�e�ƃG�l�~�[�̏Փˏ���
	void CollisionProjectileVsEnemies();

	// �m�[�h�ƃG�l�~�[�̏Փˏ���
	void CollisionAttackVsEnemies(const DirectX::XMFLOAT3& attackPosition, float nodeRadius);

	// �m�[�h�̍��W����ɕʂ̍��W�����߂�
	DirectX::XMFLOAT3 NodeToNewPosition(const char* nodeName, const DirectX::XMFLOAT3& position);

private:

	enum PlayerAnimation
	{
		
		ANIM_BACK_RUN,
		ANIM_BACK_WALK,
		ANIM_GUARD1, // ���\���n��
		ANIM_GUARD2, // ���\����
		ANIM_GUARD3, // ���󂯎�
		ANIM_DEATH1, // ���S
		ANIM_DEATH2, // ���S
		ANIM_IDLE1,
		ANIM_IDLE2,
		ANIM_IDLE3,
		ANIM_IDLE4,
		ANIM_HIT1,
		ANIM_HIT2,
		ANIM_JUMP,
		ANIM_LFET_RUN,
		ANIM_LEFT_WALK,
		ANIM_RIGHT_RUN,
		ANIM_RIGHT_WALK,
		ANIM_RUN1, // �O
		ANIM_RUN2, // �O
		ANIM_WALK, // �O
		ANIM_FALL, // ������
		ANIM_LANDING1, // ���n
		ANIM_LANDING2,
		ANIM_DODGE,
		ANIM_ATTACK1, // �P���U��
		ANIM_ATTACK2, // �P���U��
		ANIM_ATTACK3, // ��]�a��
		ANIM_ATTACK4, // �W�����v�a��(DA)
	};

	// �X�e�[�g
	enum class State
	{
		IDLE,
		WALK,
		RUN,
		DASH,
		JUMP,
		FALL,
		LAND,
		ATTACK,
		DODGE,
		DAMAGE,
		GUARD_IDLE,
		GUARD_WALK,
		GUARD_RUN,
		GUARD_DASH,
		GUARD_REACTION,
		DEATH,
	};

	PP::ppModel model = nullptr;

	Enemy* lockonEnemy = nullptr; // 

	State state = State::IDLE;

	DirectX::XMFLOAT3 lockonDirection = {}; // �����_�̌���
	DirectX::XMFLOAT3 weaponPosition = {}; // ����p�̕�����W
	DirectX::XMFLOAT3 guardOffset = { 20, 30, 20 };// �K�[�h���̊p�x����

	DirectX::XMFLOAT2 attackDirection = {};
	DirectX::XMFLOAT2 dodgeDirection = {};

	float speed = 100.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 20.0f;

	// �X�^�~�i�֘A
	float stamina = 0.0f;
	float maxStamina = 0.0f;
	float staminaTimer = 0.0f; // �񕜂��n�߂�܂ł̎���
	float staminaHeal = 0.25f;
	///////////////

	float nodeRadius = 0.5f; // �{�[������p�̔��a

	float lockonTargetChangeTime = 0.0f; // 
	float lockonTargetChangeTimeMax = 8.0f; // 

	float fallTimer = 0.0f; // ��������

	int idleTimer = 0;

	int inputTimer = 0;

	int healItem = 3; // �񕜃A�C�e����

	int runNum = 4; // ���b�N�I�����̃A�j���[�V��������

	int attackState = 0; // �U������̊Ǘ�

	bool attackCollisionFlag = false;

	bool lockonFlag = false; // ���b�N�I���J�����t���O
	bool lockonButtonFlag = false;

	bool guardFlag = false; // �K�[�h���Ă���
	bool guardedFlag = false; // �K�[�h���Ă���(���[�V�����Ǘ��̂���)

	bool cancelAttackFlag = false; // ����L�����Z���t���O(�U���֑J��)
	bool cancelDodgeFlag = false; // ����L�����Z���t���O(����֑J��)

	// Effect* hitEffect = nullptr;
	Effect* efc = nullptr;

	// ProjectileManager projectileManager;

};