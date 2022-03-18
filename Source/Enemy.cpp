#include "Enemy.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "NodeBase.h"
#include "JudgementOwner.h"
#include "ActionOwner.h"


void Enemy::DebugRender()
{
	PP::Debug::Primitive3D::DrawCylinder(wc.position, radius, height, { 0, 0, 0, 1 });
}

bool Enemy::OnMessage(const Telegram& telegram)
{
	// ���ꂼ��̃��b�Z�[�W�ɂ��킹���������L�q
	switch (telegram.msg)
	{
	case MESSAGE_TYPE::MSG_CALL_HELP:
	case MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY:
		return true;
		break;
	}
	return false;
}

void Enemy::OnDamaged()
{
	SetDamageFlag(true);

	SetReaction(true);

	// �_���[�W�X�e�[�g�֑J��
	//TransDamageState();
}

void Enemy::OnDead()
{
	SetDeathFlag(true);

	SetReaction(true);

	// Destroy();

	// ���S�X�e�[�g�֑J��
	//TransDeathState();
}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}

void Enemy::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
	territoryOrigin = origin;
	territoryRange = range;
}

void Enemy::SetRandomTargetPosition()
{
	float theta = PP::System::GetRandF(-DirectX::XM_PI, DirectX::XM_PI);
	float range = PP::System::GetRandF(0.0f, territoryRange);
	targetPosition.x = territoryOrigin.x + sinf(theta) * range;
	targetPosition.y = territoryOrigin.y;
	targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

void Enemy::MoveToTarget(float speedRate)
{
	// �^�[�Q�b�g�ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - wc.position.x;
	float vz = targetPosition.z - wc.position.z;
	float dist = sqrtf((vx * vx) + (vz * vz));
	vx /= dist;
	vz /= dist;

	// �ړ�����
	Move(vx, vz, moveSpeed * speedRate);
}

void Enemy::TurnToTarget(float speedRate, float elapsedTime)
{
	// �^�[�Q�b�g�ւ̐i�s�x�N�g�����Z�o
	float vx = targetPosition.x - wc.position.x;
	float vz = targetPosition.z - wc.position.z;
	float dist = sqrtf((vx * vx) + (vz * vz));
	vx /= dist;
	vz /= dist;

	Turn(vx, vz, turnSpeed * speedRate, elapsedTime);
}

float Enemy::GetDeltaValue(const DirectX::XMFLOAT3& value1, const DirectX::XMFLOAT3& value2)
{
	DirectX::XMFLOAT3 v1 = value1;
	DirectX::XMFLOAT3 v2 = value2;

	float vx = v1.x - v2.x;
	float vy = v1.y - v2.y;
	float vz = v1.z - v2.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	return dist;
}

bool Enemy::SearchPlayer() // �U������������͍U�����ꂽ�炵�΂炭true��Ԃ�
{
	// ���፷���݂�3����������
	const DirectX::XMFLOAT3& playerPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	float vx = playerPosition.x - wc.position.x;
	float vy = playerPosition.y - wc.position.y;
	float vz = playerPosition.z - wc.position.z;
	float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));

	// �U�������������͂��ꂽ�Ȃ�
	if (attackedFlag) return true;

	if (dist < searchRange) // ���G�͈͓����ǂ���
	{
		float distXZ = sqrtf((vx * vx) + (vz * vz));

		vx /= distXZ;
		vz /= distXZ;

		// �O�x�N�g��
		float frontX = sinf(wc.angle.y);
		float frontZ = cosf(wc.angle.y);

		// 2�̃x�N�g���̓��ςőO�㔻��
		float dot = (frontX * vx) + (frontZ * vz);
		if (dot > 0.0f)
		{
			return true;
		}
	}
	return false;
}

void Enemy::CollisionNodeVsPlayer(const char* nodeName, float nodeRadius)
{
	// �m�[�h�̈ʒu�Ƃ����蔻����s��
	PP::ppNode node = PP::Model::FindNode(model, nodeName);
	if (node != nullptr)
	{
		// �m�[�h�̃��[���h���W
		DirectX::XMFLOAT3 nodePosition = PP::Model::GetNodePosition(node);

		// �����蔻��\��
		PP::Debug::Primitive3D::DrawSphere(nodePosition, nodeRadius, { 0, 0, 1, 1 });

		// �v���C���[�Ƃ̂����蔻��
		Player& player = PlayerManager::Instance().GetPlayer();
		DirectX::XMFLOAT3 outPosition = {};
		if (PP::Collision::SphereVsCylinder(
			nodePosition, nodeRadius,
			player.GetPosition(),
			player.GetRadius(),
			player.GetHeight(),
			outPosition))
		{
			const int damage = 1;
			const float invincibleTime = 0.25f;

			// �v���C���[���K�[�h���Ă����Ȃ�
			if (player.CheckGuard(damage, invincibleTime))
			{
				// �K�[�h������
				return;
			}

			// �v���C���[�Ƀ_���[�W��^����
			if (player.ApplyDamage(damage, invincibleTime))
			{
				// �G�𐁂���΂��x�N�g�����Z�o
				DirectX::XMFLOAT3 vec = {};
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;
				float length = sqrtf((vec.x * vec.x) + (vec.z * vec.z));
				vec.x /= length;
				vec.z /= length;

				// XZ���ʂɐ�����΂��͂�������
				float power = 10.0f;
				vec.x *= power;
				vec.y = 0.0f;
				vec.z *= power;

				// ������΂�
				player.AddImpulse(vec);
			}
		}
	}
}


// Enemy01 �͉��u��
Enemy01::Enemy01()
{
	PP::Model::Make(model, "ENEMY1");

	animationMap.insert(std::make_pair("IDLE", ANIM_IDLE_NORMAL)); // �ҋ@
	animationMap.insert(std::make_pair("BATTLE_IDLE", ANIM_IDLE_BATTLE)); // �퓬�ҋ@
	animationMap.insert(std::make_pair("ATTACK1", ANIM_ATTACK1)); // �U��
	animationMap.insert(std::make_pair("WALK", ANIM_WALK_FWD)); // ���s
	animationMap.insert(std::make_pair("RUN", ANIM_RUN_FWD)); // ���s
	animationMap.insert(std::make_pair("HIT", ANIM_GET_HIT)); // ��e
	animationMap.insert(std::make_pair("DEATH", ANIM_DEATH)); // ���S

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetAngle({ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f });
	SetRadius(0.5f);
	SetHeight(1.0f);
	PP::Model::UpdateTransform(model, wc);

	attackRadius = 0.5f;
	health = 5;
	maxHealth = health;

	// �r�w�C�r�A�c���[�ݒ�
	behaviorData = new BehaviorData();
	aiTree = new BehaviorTree();

	// �\�z
	aiTree->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority_, NULL, NULL);
	aiTree->AddNode("Root", "Death", 1, BehaviorTree::SelectRule::None_, new DeathJudgement(this), new DeathAction(this));
	aiTree->AddNode("Root", "Damage", 2, BehaviorTree::SelectRule::None_, new DamageJudgement(this), new DamageAction(this));
	aiTree->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority_, new BattleJudgement(this), NULL);
	aiTree->AddNode("Root", "Scount", 4, BehaviorTree::SelectRule::Priority_, NULL, NULL);
	aiTree->AddNode("Battle", "Attack", 1, BehaviorTree::SelectRule::Random_, new AttackJudgement(this), NULL);
	aiTree->AddNode("Battle", "Pursuit", 2, BehaviorTree::SelectRule::None_, NULL, new PursuitAction(this));
	aiTree->AddNode("Attack", "Body", 0, BehaviorTree::SelectRule::None_, NULL, new BodyAction(this));
	aiTree->AddNode("Scount", "Wander", 1, BehaviorTree::SelectRule::None_, new WanderJudgement(this), new WanderAction(this));
	aiTree->AddNode("Scount", "Idle", 1, BehaviorTree::SelectRule::None_, NULL, new IdleAction(this));
}

Enemy01::~Enemy01()
{
	//delete model;
	delete aiTree;
	delete behaviorData;
}

void Enemy01::Update(float elapsedTime)
{
	// �r�w�C�r�A�c���[

	// ���ݎ��s���̃m�[�h�����邩
	if (activeNode == nullptr)
	{
		// �Ȃ���Ύ��s����m�[�h�𐄘_���AactiveNode�Ɋi�[
		activeNode = aiTree->ActiveNodeInference(this, behaviorData);
	}

	// �ēx�m�[�h�����s�������f
	if (activeNode != nullptr)
	{
		if (GetReaction()) SetRunTimer(0.0f);
		// �w���activeNodj,e��aiTree������s
		// �߂�l��activeNode���X�V
		if (GetRunTimer() == 0.0f) 
			activeNode = aiTree->Transition(this, activeNode, behaviorData);

		activeNode = aiTree->Run(this, activeNode, behaviorData, elapsedTime);
	}

	SetLockedFlag(false);

	UpdateVelocity(elapsedTime);

	UpdateInvincibleTimer(elapsedTime);

	// ���̏�������
	if (wc.position.y < -30) EnemyManager::Instance().Remove(this);

	PP::Model::UpdateAnimation(model, elapsedTime);

	PP::Model::UpdateTransform(model, wc);
}

void Enemy01::Render()
{
	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void Enemy01::RenderSub()
{
	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void Enemy01::DebugRender()
{
	PP::Debug::Primitive3D::DrawCylinder(wc.position, radius, height, { 0, 0, 0, 1 });
	//
	PP::Debug::Primitive3D::DrawCylinder(territoryOrigin, territoryRange, 1.0f, { 0, 1, 0, 1 });
	PP::Debug::Primitive3D::DrawSphere(targetPosition, radius, { 1, 1, 0, 1 });
	//// ���G�͈�
	//PP::Debug::Primitive3D::DrawCylinder(wc.position, searchRange, 1.0f, { 0, 0, 1, 1 });
	//// �U���͈�
	PP::Debug::Primitive3D::DrawCylinder(wc.position, GetAttackRange(), 1.0f, { 1, 0, 1, 1 });

}

bool Enemy01::OnMessage(const Telegram& telegram) // ��X�����̃V�X�e���ɍ����悤�ɕύX(���݂�Enemy01�ɂ��������ĂȂ�)
{
	switch (telegram.msg)
	{
	case MESSAGE_TYPE::MSG_CALL_HELP:
		if (!SearchPlayer()) // �v���C���[�������Ă��Ȃ�
		{
			// ��ԑJ��(�ǐ�)
			if (!GetCalledFlag())
				SetCalledFlag(true);
		}
		return true;
		break;
	case MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY:
		// �U���t���O��ݒ�
		SetAttackFlag(true);
		return true;
		break;
	}
	return false;
}



Enemy02::Enemy02()
{
	PP::Model::Make(model, "ENEMY2");

	animationMap.insert(std::make_pair("ATTACK1", ANIM_ATTACK1)); // �U��
	animationMap.insert(std::make_pair("DEATH", ANIM_DEATH1)); // ���S
	animationMap.insert(std::make_pair("HIT", ANIM_HIT)); // ��e
	animationMap.insert(std::make_pair("IDLE", ANIM_IDLE)); // �ҋ@
	animationMap.insert(std::make_pair("WALK", ANIM_RUN)); // ���s
	animationMap.insert(std::make_pair("RUN", ANIM_RUN)); // ���s

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetAngle({ 0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f });
	SetRadius(1.5f);
	SetHeight(2.25f);
	PP::Model::UpdateTransform(model, wc);

	// �r�w�C�r�A�c���[�ݒ�
	behaviorData = new BehaviorData();
	aiTree = new BehaviorTree();

	// �\�z
	aiTree->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority_, NULL, NULL);
	aiTree->AddNode("Root", "Death", 1, BehaviorTree::SelectRule::None_, new DeathJudgement(this), new DeathAction(this));
	aiTree->AddNode("Root", "Damage", 2, BehaviorTree::SelectRule::None_, new DamageJudgement(this), new DamageAction(this));
	aiTree->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority_, new BattleJudgement(this), NULL);
	aiTree->AddNode("Root", "Scount", 4, BehaviorTree::SelectRule::Priority_, NULL, NULL);
	aiTree->AddNode("Battle", "Attack", 1, BehaviorTree::SelectRule::Random_, new AttackJudgement(this), NULL);
	aiTree->AddNode("Battle", "Pursuit", 2, BehaviorTree::SelectRule::None_, NULL, new PursuitAction(this));
	aiTree->AddNode("Attack", "Body", 0, BehaviorTree::SelectRule::None_, NULL, new BodyAction(this));
	aiTree->AddNode("Scount", "Wander", 1, BehaviorTree::SelectRule::None_, new WanderJudgement(this), new WanderAction(this));
	aiTree->AddNode("Scount", "Idle", 1, BehaviorTree::SelectRule::None_, NULL, new IdleAction(this));
}

Enemy02::~Enemy02()
{
	//delete model;
	delete aiTree;
	delete behaviorData;
}

void Enemy02::Update(float elapsedTime)
{
	// ���ݎ��s���̃m�[�h�����邩
	if (activeNode == nullptr)
	{
		// �Ȃ���Ύ��s����m�[�h�𐄘_���AactiveNode�Ɋi�[
		activeNode = aiTree->ActiveNodeInference(this, behaviorData);
	}

	// �ēx�m�[�h�����s�������f
	if (activeNode != nullptr)
	{
		if (GetReaction()) SetRunTimer(0.0f);
		// �w���activeNodj,e��aiTree������s
		// �߂�l��activeNode���X�V
		if (GetRunTimer() == 0.0f)
			activeNode = aiTree->Transition(this, activeNode, behaviorData);

		activeNode = aiTree->Run(this, activeNode, behaviorData, elapsedTime);
	}

	SetLockedFlag(false);

	UpdateVelocity(elapsedTime);

	UpdateInvincibleTimer(elapsedTime);

	PP::Model::UpdateAnimation(model, elapsedTime);

	PP::Model::UpdateTransform(model, wc);
}

void Enemy02::Render()
{
	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void Enemy02::RenderSub()
{
	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void Enemy02::DebugRender()
{
	PP::Debug::Primitive3D::DrawCylinder(wc.position, radius, height, { 0, 0, 0, 1 });

	PP::Debug::Primitive3D::DrawCylinder(territoryOrigin, territoryRange, 1.0f, { 0, 1, 0, 1 });
	PP::Debug::Primitive3D::DrawSphere(targetPosition, radius, { 1, 1, 0, 1 });
	// ���G�͈�
	PP::Debug::Primitive3D::DrawCylinder(wc.position, searchRange, 1.0f, { 0, 0, 1, 1 });
	// �U���͈�
	PP::Debug::Primitive3D::DrawCylinder(wc.position, GetAttackRange(), 1.0f, { 1, 0, 1, 1 });
}