#include "ActionOwner.h"
#include "Enemy.h"
#include "PlayerManager.h"
#include "StageObjectManager.h"
#include "CheckPoint.h"
#include "MetaAI.h"
#include "Navigation.h"

// 徘徊行動
ActionBase::State WanderAction::Transition()
{
	// 目的地をランダムで設定
	owner->SetRandomTargetPosition();

	// 時間を設定(減らしていない)
	owner->SetRunTimer(1.0f);

	// モーション再生
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("WALK"), true);

	return ActionBase::State::Complete_;
}

ActionBase::State WanderAction::Run(float elapsedTime)
{
	// 目的地点までのXZ平面での距離判定
	DirectX::XMFLOAT3 position = owner->GetPosition();
	DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition();
	float vx = targetPosition.x - position.x;
	float vz = targetPosition.z - position.z;
	float distSq = vx * vx + vz * vz;

	// 目的地へ移動
	float speedRate = 0.5f;
	owner->MoveToTarget(speedRate);
	owner->TurnToTarget(speedRate, elapsedTime);

	// 目的へ着いた
	float radius = owner->GetRadius();
	if (distSq < radius * radius)
	{
		owner->SetRunTimer(0.0f);
		return ActionBase::State::Complete_;
	}

	// 誰かがプレイヤー探索に成功したら
	if (owner->SearchPlayer() || owner->GetCalledFlag())
	{
		owner->SetRunTimer(0.0f);

		return ActionBase::State::Complete_;
	}

	// 実行中
	return ActionBase::State::Run_;
}

// 待機行動
ActionBase::State IdleAction::Transition()
{
	// 時間をランダムで設定
	owner->SetRunTimer(PP::System::GetRandF(3.0f, 5.0f));

	// モーション再生
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("IDLE"), true);

	return ActionBase::State::Complete_;
}

ActionBase::State IdleAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();

	runTimer -= elapsedTime;

	// タイマー更新
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

// 追跡行動
ActionBase::State PursuitAction::Transition()
{
	// 時間をランダムで設定
	owner->SetRunTimer(PP::System::GetRandF(10.0f, 15.0f));

	// 助けを求める
	if (!owner->GetCalledFlag())
		Meta::Instance().SendMessaging(owner->GetId(), static_cast<int>(Meta::Identity::Meta),
			MESSAGE_TYPE::MSG_CALL_HELP);

	// 攻撃権をなくす
	Meta::Instance().SendMessaging(owner->GetId(),
		static_cast<int>(Meta::Identity::Meta), MESSAGE_TYPE::MSG_CHANGE_ATTACK_AUTHORITY);

	// モーション再生
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("RUN"), true);

	return ActionBase::State::Complete_;
}

ActionBase::State PursuitAction::Run(float elapsedTime)
{
	float runTimer = owner->GetRunTimer();

	runTimer -= elapsedTime;

	// タイマー更新
	owner->SetRunTimer(runTimer);

	DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	Navigation::Instance().SetTarget(playerPosition); // ナビ先をプレイヤーに設定
	DirectX::XMFLOAT3 naviPosition = {};
	Navigation::Instance().FindRoot(owner->GetPosition(), naviPosition); // ルート検索

	owner->SetTargetPosition(naviPosition); // 目的地の設定

	DirectX::XMFLOAT3 position = owner->GetPosition();
	//DirectX::XMFLOAT3 targetPosition = owner->GetTargetPosition(); // ナビ座標

	// ナビ座標と自分の座標の差分
	float vx = naviPosition.x - position.x;
	float vy = naviPosition.y - position.y;
	float vz = naviPosition.z - position.z;
	float dist = sqrtf(vx * vx + vy * vy + vz * vz);

	float speedRate = 1.0f;
	// ナビ座標とプレイヤー座標の差分
	float delta = owner->GetDeltaValue(playerPosition, naviPosition);
	// 攻撃範囲内なら回転のみ
	if (dist < owner->GetAttackRange() && delta < owner->GetAttackRange())
	{
		owner->TurnToTarget(speedRate * 2.0f, elapsedTime);

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
		// 向きがある程度あっていれば遷移
		if (dot < DirectX::XMConvertToRadians(10)) 
		{
			owner->SetRunTimer(0.0f);

			// メタAIに攻撃権を求める
			Meta::Instance().SendMessaging(owner->GetId(),
				static_cast<int>(Meta::Identity::Meta), MESSAGE_TYPE::MSG_SEEK_ATTACK_AUTHORITY);

			owner->SetCalledFlag(false);

			return ActionBase::State::Complete_;
		}
	}
	else // 攻撃範囲外なら普通に追跡
	{
		// 目的地へ移動
		owner->MoveToTarget(speedRate);
		owner->TurnToTarget(speedRate, elapsedTime);
	}

	if (runTimer <= 0.0f)
	{
		owner->SetRunTimer(0.0f);

		owner->SetCalledFlag(false);

		// 追跡終了
		owner->SetAttackedFlag(false);

		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// 攻撃行動
ActionBase::State BodyAction::Transition()
{
	// 時間設定
	owner->SetRunTimer(0.5f * 6);

	// モーション再生
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("ATTACK1"), false);

	return ActionBase::State::Complete_;
}

ActionBase::State BodyAction::Run(float elapsedTime)
{
	// 任意のアニメーション再生区間でのみ衝突判定処理
	float animationTime = PP::Model::GetCurrentAnimationSeconds(owner->GetModel());
	if (animationTime >= 0.1f && animationTime <= 0.35f)
	{
		// 指定ノードとプレイヤーの衝突処理
		owner->CollisionNodeVsPlayer("EyeBall", owner->GetAttackRadius());
	}

	float runTimer = owner->GetRunTimer();

	runTimer -= elapsedTime;

	// タイマー更新
	owner->SetRunTimer(runTimer);

	// 目標地点をプレイヤーに設定
	owner->SetTargetPosition(PlayerManager::Instance().GetPlayer().GetPosition());

	// 目的地へ移動
	//owner->MoveToTarget(1.0f, elapsedTime);

	if (runTimer <= 0.0f)
	{
		owner->SetRunTimer(0.0f);

		// 攻撃権をなくす
		Meta::Instance().SendMessaging(owner->GetId(),
			static_cast<int>(Meta::Identity::Meta), MESSAGE_TYPE::MSG_CHANGE_ATTACK_AUTHORITY);

		owner->SetAttackFlag(false);

		// 追跡続行
		owner->SetAttackedFlag(true);

		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// ダメージ行動
ActionBase::State DamageAction::Transition()
{
	// モーション再生
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("HIT"), false);

	owner->SetRunTimer(1.0f);
	owner->SetReaction(false);

	return ActionBase::State::Complete_;
}

ActionBase::State DamageAction::Run(float elapsedTime)
{
	// モーション再生が終わっているなら
	if (!PP::Model::IsPlayAnimation(owner->GetModel()))
	{
		owner->SetRunTimer(0.0f);
		owner->SetDamageFlag(false);
		return ActionBase::State::Complete_;
	}

	return ActionBase::State::Run_;
}

// 死亡行動
ActionBase::State DeathAction::Transition()
{
	// モーション再生
	PP::Model::PlayAnimation(owner->GetModel(), owner->GetAnimIndex("DEATH"), false);

	owner->SetRunTimer(1.0f);
	owner->SetReaction(false);

	return ActionBase::State::Complete_;
}

ActionBase::State DeathAction::Run(float elapsedTime)
{
	// モーション再生が終わっているなら
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

