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
	// それぞれのメッセージにあわせた処理を記述
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

	// ダメージステートへ遷移
	//TransDamageState();
}

void Enemy::OnDead()
{
	SetDeathFlag(true);

	SetReaction(true);

	// Destroy();

	// 死亡ステートへ遷移
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
	// ターゲットへの進行ベクトルを算出
	float vx = targetPosition.x - wc.position.x;
	float vz = targetPosition.z - wc.position.z;
	float dist = sqrtf((vx * vx) + (vz * vz));
	vx /= dist;
	vz /= dist;

	// 移動処理
	Move(vx, vz, moveSpeed * speedRate);
}

void Enemy::TurnToTarget(float speedRate, float elapsedTime)
{
	// ターゲットへの進行ベクトルを算出
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

bool Enemy::SearchPlayer() // 攻撃するもしくは攻撃されたらしばらくtrueを返す
{
	// 高低差込みの3軸距離判定
	const DirectX::XMFLOAT3& playerPosition = PlayerManager::Instance().GetPlayer().GetPosition();
	float vx = playerPosition.x - wc.position.x;
	float vy = playerPosition.y - wc.position.y;
	float vz = playerPosition.z - wc.position.z;
	float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));

	// 攻撃したもしくはされたなら
	if (attackedFlag) return true;

	if (dist < searchRange) // 索敵範囲内かどうか
	{
		float distXZ = sqrtf((vx * vx) + (vz * vz));

		vx /= distXZ;
		vz /= distXZ;

		// 前ベクトル
		float frontX = sinf(wc.angle.y);
		float frontZ = cosf(wc.angle.y);

		// 2つのベクトルの内積で前後判定
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
	// ノードの位置とあたり判定を行う
	PP::ppNode node = PP::Model::FindNode(model, nodeName);
	if (node != nullptr)
	{
		// ノードのワールド座標
		DirectX::XMFLOAT3 nodePosition = PP::Model::GetNodePosition(node);

		// 当たり判定表示
		PP::Debug::Primitive3D::DrawSphere(nodePosition, nodeRadius, { 0, 0, 1, 1 });

		// プレイヤーとのあたり判定
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

			// プレイヤーがガードしていたなら
			if (player.CheckGuard(damage, invincibleTime))
			{
				// ガードをした
				return;
			}

			// プレイヤーにダメージを与える
			if (player.ApplyDamage(damage, invincibleTime))
			{
				// 敵を吹っ飛ばすベクトルを算出
				DirectX::XMFLOAT3 vec = {};
				vec.x = outPosition.x - nodePosition.x;
				vec.z = outPosition.z - nodePosition.z;
				float length = sqrtf((vec.x * vec.x) + (vec.z * vec.z));
				vec.x /= length;
				vec.z /= length;

				// XZ平面に吹っ飛ばす力をかける
				float power = 10.0f;
				vec.x *= power;
				vec.y = 0.0f;
				vec.z *= power;

				// 吹っ飛ばす
				player.AddImpulse(vec);
			}
		}
	}
}


// Enemy01 は仮置き
Enemy01::Enemy01()
{
	PP::Model::Make(model, "ENEMY1");

	animationMap.insert(std::make_pair("IDLE", ANIM_IDLE_NORMAL)); // 待機
	animationMap.insert(std::make_pair("BATTLE_IDLE", ANIM_IDLE_BATTLE)); // 戦闘待機
	animationMap.insert(std::make_pair("ATTACK1", ANIM_ATTACK1)); // 攻撃
	animationMap.insert(std::make_pair("WALK", ANIM_WALK_FWD)); // 歩行
	animationMap.insert(std::make_pair("RUN", ANIM_RUN_FWD)); // 走行
	animationMap.insert(std::make_pair("HIT", ANIM_GET_HIT)); // 被弾
	animationMap.insert(std::make_pair("DEATH", ANIM_DEATH)); // 死亡

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetAngle({ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f });
	SetRadius(0.5f);
	SetHeight(1.0f);
	PP::Model::UpdateTransform(model, wc);

	attackRadius = 0.5f;
	health = 5;
	maxHealth = health;

	// ビヘイビアツリー設定
	behaviorData = new BehaviorData();
	aiTree = new BehaviorTree();

	// 構築
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
	// ビヘイビアツリー

	// 現在実行中のノードがあるか
	if (activeNode == nullptr)
	{
		// なければ実行するノードを推論し、activeNodeに格納
		activeNode = aiTree->ActiveNodeInference(this, behaviorData);
	}

	// 再度ノードが実行中か判断
	if (activeNode != nullptr)
	{
		if (GetReaction()) SetRunTimer(0.0f);
		// 指定のactiveNodj,eをaiTreeから実行
		// 戻り値でactiveNodeを更新
		if (GetRunTimer() == 0.0f) 
			activeNode = aiTree->Transition(this, activeNode, behaviorData);

		activeNode = aiTree->Run(this, activeNode, behaviorData, elapsedTime);
	}

	SetLockedFlag(false);

	UpdateVelocity(elapsedTime);

	UpdateInvincibleTimer(elapsedTime);

	// 仮の消去処理
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
	//// 索敵範囲
	//PP::Debug::Primitive3D::DrawCylinder(wc.position, searchRange, 1.0f, { 0, 0, 1, 1 });
	//// 攻撃範囲
	PP::Debug::Primitive3D::DrawCylinder(wc.position, GetAttackRange(), 1.0f, { 1, 0, 1, 1 });

}

bool Enemy01::OnMessage(const Telegram& telegram) // 後々自分のシステムに合うように変更(現在はEnemy01にしかつくってない)
{
	switch (telegram.msg)
	{
	case MESSAGE_TYPE::MSG_CALL_HELP:
		if (!SearchPlayer()) // プレイヤーを見つけていない
		{
			// 状態遷移(追跡)
			if (!GetCalledFlag())
				SetCalledFlag(true);
		}
		return true;
		break;
	case MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY:
		// 攻撃フラグを設定
		SetAttackFlag(true);
		return true;
		break;
	}
	return false;
}



Enemy02::Enemy02()
{
	PP::Model::Make(model, "ENEMY2");

	animationMap.insert(std::make_pair("ATTACK1", ANIM_ATTACK1)); // 攻撃
	animationMap.insert(std::make_pair("DEATH", ANIM_DEATH1)); // 死亡
	animationMap.insert(std::make_pair("HIT", ANIM_HIT)); // 被弾
	animationMap.insert(std::make_pair("IDLE", ANIM_IDLE)); // 待機
	animationMap.insert(std::make_pair("WALK", ANIM_RUN)); // 歩行
	animationMap.insert(std::make_pair("RUN", ANIM_RUN)); // 走行

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetAngle({ 0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f });
	SetRadius(1.5f);
	SetHeight(2.25f);
	PP::Model::UpdateTransform(model, wc);

	// ビヘイビアツリー設定
	behaviorData = new BehaviorData();
	aiTree = new BehaviorTree();

	// 構築
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
	// 現在実行中のノードがあるか
	if (activeNode == nullptr)
	{
		// なければ実行するノードを推論し、activeNodeに格納
		activeNode = aiTree->ActiveNodeInference(this, behaviorData);
	}

	// 再度ノードが実行中か判断
	if (activeNode != nullptr)
	{
		if (GetReaction()) SetRunTimer(0.0f);
		// 指定のactiveNodj,eをaiTreeから実行
		// 戻り値でactiveNodeを更新
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
	// 索敵範囲
	PP::Debug::Primitive3D::DrawCylinder(wc.position, searchRange, 1.0f, { 0, 0, 1, 1 });
	// 攻撃範囲
	PP::Debug::Primitive3D::DrawCylinder(wc.position, GetAttackRange(), 1.0f, { 1, 0, 1, 1 });
}