#include "JudgementOwner.h"
#include "PlayerManager.h"


// WanderNode�ɑJ�ڂł��邩����
bool WanderJudgement::Judgement()
{
	// �ړI�n�܂ł�XZ���ʂł̋�������
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// �ړI�n���痣��Ă���ꍇ
	float radius = owner->GetRadius();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}

// BattleNode�ɑJ�ڂł��邩����
bool BattleJudgement::Judgement()
{
	// �v���C���[�������邩
	if (owner->SearchPlayer() || owner->GetCalledFlag())
	{
		return true;
	}
	return false;
}

// AttackNode�J�ڂł��邩����
bool AttackJudgement::Judgement()
{
	// �ΏۂƂ̋������Z�o
	//DirectX::XMFLOAT3 position = owner->GetPosition();
	//DirectX::XMFLOAT3 targetPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	//
	//float vx = targetPosition.x - position.x;
	//float vy = targetPosition.y - position.y;
	//float vz = targetPosition.z - position.z;
	//float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	
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

	// �v���C���[���W�ƃi�r���W�̍���
	float dist = owner->GetDeltaValue(playerPosition, targetPosition);

	// �v���C���[���W�ƓG�̃i�r���W�����ȓ�(�قڍ����Ă���)
	if (owner->GetAttackFlag() && dist < owner->GetAttackRadius())
	{
		if (dot < DirectX::XMConvertToRadians(10))
			return true;
	}

	return false;
}

// DamageNode�ɑJ�ڂł��锻��
bool DamageJudgement::Judgement()
{
	// damageFlag�Ŕ��f
	
	return owner->GetDamageFlag();
}

// DeathNode�ɑJ�ڂł��邩����
bool DeathJudgement::Judgement()
{
	// deathFlag�Ŕ��f
	return owner->GetDeathFlag();
}