#include "JudgementOwner.h"
#include "PlayerManager.h"


// WanderNodeに遷移できるか判定
bool WanderJudgement::Judgement()
{
	// 目的地までのXZ平面での距離判定
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// 目的地から離れている場合
	float radius = owner->GetRadius();
	if (distSq > radius * radius)
	{
		return true;
	}

	return false;
}

// BattleNodeに遷移できるか判定
bool BattleJudgement::Judgement()
{
	// プレイヤーが見つかるか
	if (owner->SearchPlayer() || owner->GetCalledFlag())
	{
		return true;
	}
	return false;
}

// AttackNode遷移できるか判定
bool AttackJudgement::Judgement()
{
	// 対象との距離を算出
	//DirectX::XMFLOAT3 position = owner->GetPosition();
	//DirectX::XMFLOAT3 targetPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	//
	//float vx = targetPosition.x - position.x;
	//float vy = targetPosition.y - position.y;
	//float vz = targetPosition.z - position.z;
	//float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	
	// 二つのベクトルの角度差
	float dot = 0.0f;
	{
		// 前方ベクトル
		DirectX::XMFLOAT3 front = PP::Model::GetForward(owner->GetModel());

		// エネミー->プレイヤー
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

	// プレイヤー座標とナビ座標の差分
	float dist = owner->GetDeltaValue(playerPosition, targetPosition);

	// プレイヤー座標と敵のナビ座標が一定以内(ほぼ合っている)
	if (owner->GetAttackFlag() && dist < owner->GetAttackRadius())
	{
		if (dot < DirectX::XMConvertToRadians(10))
			return true;
	}

	return false;
}

// DamageNodeに遷移できる判定
bool DamageJudgement::Judgement()
{
	// damageFlagで判断
	
	return owner->GetDamageFlag();
}

// DeathNodeに遷移できるか判定
bool DeathJudgement::Judgement()
{
	// deathFlagで判断
	return owner->GetDeathFlag();
}