#include "ActionOwner.h"
#include "Enemy.h"
#include "PlayerManager.h"
#include "StageObjectManager.h"
#include "CheckPoint.h"
#include "MetaAI.h"
#include "Navigation.h"

// �p�j�s��
ActionBase::State WanderAction::Transition()
{
	// �ړI�n�������_���Őݒ�
	owner->SetRandomTargetPosition();

	// ���Ԃ�ݒ�(���炵�Ă��Ȃ�)
	owner->SetRunTimer(1.0f);

	// ���[�V�����Đ�
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("WALK"), true);

	return ActionBase::State::Complete_;
}

ActionBase::State WanderAction::Run(float elapsedTime)
{
	// �ړI�n�_�܂ł�XZ���ʂł̋�������
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// �ړI�n�ֈړ�
	float speedRate = 0.5f;
	owner->MoveToTarget(speedRate);
	owner->TurnToTarget(speedRate, elapsedTime);

	// �ړI�֒�����
	float radius = owner->GetRadius();
	if (distSq < radius * radius)
	{
		owner->SetRunTimer(0.0f);
		return ActionBase::State::Complete_;
	}

	// �N�����v���C���[�T���ɐ���������
	if (owner->SearchPlayer() || owner->GetCalledFlag())
	{
		owner->SetRunTimer(0.0f);

		return ActionBase::State::Complete_;
	}

	// ���s��
	return ActionBase::State::Run_;
}

// �ҋ@�s��
ActionBase::State IdleAction::Transition()
{
	// ���Ԃ������_���Őݒ�
	owner->SetRunTimer(PP::System::GetRandF(3.0f, 5.0f));

	// ���[�V�����Đ�
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("IDLE"), true);

	return ActionBase::State::Complete_;
}

ActionBase::State IdleAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();

	runTimer -= elapsedTime;

	// �^�C�}�[�X�V
	owner->SetRunTimer(runTimer);

	if (runTimer <= 0.0f)
	{
		owner->SetRandomTargetPosition();
		owner->SetRunTimer(0.0f);
		return ActionBase::State::Complete_;
	}

	if (owner->SearchPlayer())
	{
		owner->SetRunTimer(0.0f);
		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// �ǐՍs��
ActionBase::State PursuitAction::Transition()
{
	// ���Ԃ������_���Őݒ�
	owner->SetRunTimer(PP::System::GetRandF(10.0f, 15.0f));

	// ���������߂�
	if (!owner->GetCalledFlag())
		Meta::Instance().SendMessaging(owner->GetId(), static_cast<int>(Meta::Identity::Meta),
			MESSAGE_TYPE::MSG_CALL_HELP);

	// �U�������Ȃ���
	Meta::Instance().SendMessaging(owner->GetId(),
		static_cast<int>(Meta::Identity::Meta), MESSAGE_TYPE::MSG_CHANGE_ATTACK_AUTHORITY);

	// ���[�V�����Đ�
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("RUN"), true);

	return ActionBase::State::Complete_;
}

ActionBase::State PursuitAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();

	runTimer -= elapsedTime;

	// �^�C�}�[�X�V
	owner->SetRunTimer(runTimer);

	DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	Navigation::Instance().SetTarget(playerPosition); // �i�r����v���C���[�ɐݒ�
	DirectX::XMFLOAT3 naviPosition = {};
	Navigation::Instance().FindRoot(owner->GetPosition(), naviPosition); // ���[�g����

	owner->SetTargetPosition(naviPosition); // �ړI�n�̐ݒ�

	DirectX::XMFLOAT3 position = owner->GetPosition();
	//DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition(); // �i�r���W

	// �i�r���W�Ǝ����̍��W�̍���
	float vx = naviPosition.x - position.x;
	float vy = naviPosition.y - position.y;
	float vz = naviPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	float speedRate = 1.0f;
	// �i�r���W�ƃv���C���[���W�̍���
	float delta = owner->GetDeltaValue(playerPosition, naviPosition);
	// �U���͈͓��Ȃ��]�̂�
	if (dist < owner->GetAttackRange() && delta < owner->GetAttackRange())
	{
		owner->TurnToTarget(speedRate * 2.0f, elapsedTime);

		// ��̃x�N�g���̊p�x��
		float dot = 0.0f;
		{
			// �O���x�N�g��
			DirectX::XMFLOAT3 front = PP::Model::GetForward(owner->GetModel());

			// �G�l�~�[->�v���C���[
			DirectX::XMFLOAT3 v = {
			playerPosition.x - owner->GetPosition().x,
			playerPosition.y - owner->GetPosition().y,
			playerPosition.z - owner->GetPosition().z };

			DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&front);
			DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&v);
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(V), DirectX::XMVector3Normalize(Front));
			DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(DirectX::XMVector3Normalize(V), DirectX::XMVector3Normalize(Front));


			DirectX::XMFLOAT3 cross = {};
			DirectX::XMStoreFloat(&dot, Dot);
			DirectX::XMStoreFloat3(&cross, Cross);
			dot = acosf(dot);
		}
		// ������������x�����Ă���ΑJ��
		if (dot < DirectX::XMConvertToRadians(10)) 
		{
			owner->SetRunTimer(0.0f);

			// ���^AI�ɍU���������߂�
			Meta::Instance().SendMessaging(owner->GetId(),
				static_cast<int>(Meta::Identity::Meta), MESSAGE_TYPE::MSG_SEEK_ATTACK_AUTHORITY);

			owner->SetCalledFlag(false);

			return ActionBase::State::Complete_;
		}
	}
	else // �U���͈͊O�Ȃ畁�ʂɒǐ�
	{
		// �ړI�n�ֈړ�
		owner->MoveToTarget(speedRate);
		owner->TurnToTarget(speedRate, elapsedTime);
	}

	if (runTimer <= 0.0f)
	{
		owner->SetRunTimer(0.0f);

		owner->SetCalledFlag(false);

		// �ǐՏI��
		owner->SetAttackedFlag(false);

		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// �U���s��
ActionBase::State BodyAction::Transition()
{
	// ���Ԑݒ�
	owner->SetRunTimer(0.5f * 6);

	// ���[�V�����Đ�
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("ATTACK1"), false);

	return ActionBase::State::Complete_;
}

ActionBase::State BodyAction::Run(float elapsedTime)
{
	// �C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��
	float animationTime = PP::Model::GetCurrentAnimationSeconds(owner->GetModel());
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		// �w��m�[�h�ƃv���C���[�̏Փˏ���
		owner->CollisionNodeVsPlayer("EyeBall", owner->GetAttackRadius());
	}

	float runTimer = owner->GetRunTimer();

	runTimer -= elapsedTime;

	// �^�C�}�[�X�V
	owner->SetRunTimer(runTimer);

	// �ڕW�n�_���v���C���[�ɐݒ�
	owner->SetTargetPosition(PlayerManager::Instance().GetPlayer().GetPosition());

	// �ړI�n�ֈړ�
	//owner->MoveToTarget(1.0f, elapsedTime);

	if (runTimer <= 0.0f)
	{
		owner->SetRunTimer(0.0f);

		// �U�������Ȃ���
		Meta::Instance().SendMessaging(owner->GetId(),
			static_cast<int>(Meta::Identity::Meta), MESSAGE_TYPE::MSG_CHANGE_ATTACK_AUTHORITY);

		owner->SetAttackFlag(false);

		// �ǐՑ��s
		owner->SetAttackedFlag(true);

		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// �_���[�W�s��
ActionBase::State DamageAction::Transition()
{
	// ���[�V�����Đ�
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("HIT"), false);

	owner->SetRunTimer(1.0f);
	owner->SetReaction(false);

	return ActionBase::State::Complete_;
}

ActionBase::State DamageAction::Run(float elapsedTime)
{
	// ���[�V�����Đ����I����Ă���Ȃ�
	if (!PP::Model::IsPlayAnimation(owner->GetModel()))
	{
		owner->SetRunTimer(0.0f);
		owner->SetDamageFlag(false);
		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// ���S�s��
ActionBase::State DeathAction::Transition()
{
	// ���[�V�����Đ�
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("DEATH"), false);

	owner->SetRunTimer(1.0f);
	owner->SetReaction(false);

	return ActionBase::State::Complete_;
}

ActionBase::State DeathAction::Run(float elapsedTime)
{
	// ���[�V�����Đ����I����Ă���Ȃ�
	if (!PP::Model::IsPlayAnimation(owner->GetModel()))
	{
		owner->SetRunTimer(0.0f);
		owner->SetDeathFlag(false);
		owner->Destroy();

		if (owner->Checkboss())
		{
			StageObjectManager::Instance().Register(new CheckPoint());
		}

		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

