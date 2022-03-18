#pragma once
#include <map>
#include "Object.h"
//#include "MetaAI.h"
#include "Telegram.h"

class BehaviorTree;
class BehaviorData;
class NodeBase;

// �G�l�~�[
class Enemy : public Object
{
public:

	Enemy() {}
	~Enemy() override {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render() = 0;

	virtual void RenderSub() = 0;

	virtual void DebugRender();

	// ���b�Z�[�W��M���̏���
	virtual bool OnMessage(const Telegram& mesg);

	// �_���[�W��
	void OnDamaged() override;

	// ���S��
	void OnDead() override;

	void Destroy();

	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	// �^�[�Q�b�g�ʒu�̐ݒ�(�����_��)
	void SetRandomTargetPosition();

	// �ڕW�n�_�ֈړ�
	void MoveToTarget(float speedRate);

	// �ړI�n�_�̕����ɉ�]
	void TurnToTarget(float speedRate, float elapsedTime);

	// 1 - 2�̒l�̑傫��
	float GetDeltaValue(const DirectX::XMFLOAT3& value1, const DirectX::XMFLOAT3& value2);

	// �v���C���[
	bool SearchPlayer();

	// ���f���擾
	PP::ppModel GetModel() { return model; }

	// �^�[�Q�b�g�|�W�V�����ݒ�
	void SetTargetPosition(DirectX::XMFLOAT3 position) { targetPosition = position; }

	// �^�[�Q�b�g�|�W�V�����擾
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	// �U���͈͎擾
	const float GetAttackRange() { return attackRange; }

	// �U�����̃m�[�h�̔��a
	const float GetAttackRadius() { return attackRadius; }

	// ���s�^�C�}�[�ݒ�
	void SetRunTimer(float timer) { runTimer = timer; }

	// ���s�^�C�}�[�擾
	const float GetRunTimer() { return runTimer; }

	// ���A�N�V�����t���O�ݒ�
	void SetReaction(bool flag) { reactionFlag = flag; }

	// ���A�N�V�����t���O�擾
	const bool GetReaction() { return reactionFlag; }

	// �_���[�W�t���O�ݒ�
	void SetDamageFlag(bool flag) { damageFlag = flag; }

	// �_���[�W�t���O�擾
	const bool GetDamageFlag() { return damageFlag; }

	// ���S�t���O�ݒ�
	void SetDeathFlag(bool flag) { deathFlag = flag; }

	// ���S�t���O�擾
	const bool GetDeathFlag() { return deathFlag; }

	// ���b�N�I������Ă�t���O�ݒ�
	void SetLockedFlag(bool flag) { lockedFlag = flag; }

	// ���b�N�I������Ă�t���O�擾
	const bool GetLockedFlag() { return lockedFlag; }

	// �U���ł���t���O�ݒ�
	void SetAttackFlag(bool flag) { attackFlag = flag; }

	// �U���ł���t���O�擾
	const bool GetAttackFlag() { return attackFlag; }

	// �U�������������͂��ꂽ�t���O
	void SetAttackedFlag(bool flag) { attackedFlag = flag; }

	// ���̓G�ɌĂ΂ꂽ�t���O�ݒ�
	void SetCalledFlag(bool flag) { calledFlag = flag; }

	// ���̓G�ɌĂ΂ꂽ�t���O�擾
	const bool GetCalledFlag() { return calledFlag; }

	// �{�X�ɐݒ�
	void SetBoss() { bossFlag = true; }

	// �{�X���ǂ���
	const bool Checkboss() { return bossFlag; }

	// ID�ݒ�
	void SetId(int id) { this->id = id; }

	// ID�擾
	int	GetId() { return id; }

	// �m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	int GetAnimIndex(std::string key) { return animationMap.at(key); }

protected:

	PP::ppModel model = nullptr;

	DirectX::XMFLOAT3 targetPosition = {};
	DirectX::XMFLOAT3 territoryOrigin = { 6, 0, 32 };

	int id = 0;

	float territoryRange = 14.0f;
	float moveSpeed = 3.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float runTimer = 0.0f;
	float searchRange = 5.0f;

	float attackRange = 1.5f;
	float attackRadius = 0.2f; // �U�����ɔ�����Ƃ�m�[�h�̔��a(�����l�̓X���C���̍U�����a)

	BehaviorTree* aiTree = nullptr;
	BehaviorData* behaviorData = nullptr;
	NodeBase* activeNode = nullptr;

	std::map<std::string, int> animationMap;

private:

	bool lockedFlag = false; // ���b�N�I������Ă���
	bool reactionFlag = false;
	bool damageFlag = false;
	bool deathFlag = false;
	bool attackFlag = false;
	bool attackedFlag = false;
	bool calledFlag = false;
	bool bossFlag = false; // �{�X�G���ǂ���

};



// ��U�����ɉ��u��(��قǕʃw�b�_�[�Ɉڂ�)
// �X���C��
class Enemy01 : public Enemy
{
public:

	Enemy01();
	~Enemy01() override;

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	// ���b�Z�[�W��M���̏���
	bool OnMessage(const Telegram& telegram) override;

private:

	enum Enemy01Animation
	{
		ANIM_IDLE_NORMAL,
		ANIM_IDLE_BATTLE,
		ANIM_ATTACK1,
		ANIM_ATTACK2,
		ANIM_WALK_FWD,
		ANIM_WALK_BWD,
		ANIM_WALK_LEFT,
		ANIM_WALK_RIGHT,
		ANIM_RUN_FWD,
		ANIM_SENSE_SOMTHING_ST,
		ANIM_SENSE_SOMTHING_PRT,
		ANIM_TAUNT,
		ANIM_VICTORY,
		ANIM_GET_HIT,
		ANIM_DIZZY,
		ANIM_DEATH,
	};

};

class Enemy02 : public Enemy
{
public:

	Enemy02();
	~Enemy02() override;

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

private:

	enum Enemy02Animation
	{
		ANIM_ATTACK1,
		ANIM_ATTACK2,
		ANIM_DEATH1,
		ANIM_DEATH2,
		ANIM_HIT,
		ANIM_IDLE,
		ANIM_JUMP,
		ANIM_RUN,
	};

};

