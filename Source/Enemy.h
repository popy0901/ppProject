#pragma once
#include <map>
#include "Object.h"
//#include "MetaAI.h"
#include "Telegram.h"

class BehaviorTree;
class BehaviorData;
class NodeBase;

// エネミー
class Enemy : public Object
{
public:

	Enemy() {}
	~Enemy() override {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render() = 0;

	virtual void RenderSub() = 0;

	virtual void DebugRender();

	// メッセージ受信時の処理
	virtual bool OnMessage(const Telegram& mesg);

	// ダメージ時
	void OnDamaged() override;

	// 死亡時
	void OnDead() override;

	void Destroy();

	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

	// ターゲット位置の設定(ランダム)
	void SetRandomTargetPosition();

	// 目標地点へ移動
	void MoveToTarget(float speedRate);

	// 目的地点の方向に回転
	void TurnToTarget(float speedRate, float elapsedTime);

	// 1 - 2の値の大きさ
	float GetDeltaValue(const DirectX::XMFLOAT3& value1, const DirectX::XMFLOAT3& value2);

	// プレイヤー
	bool SearchPlayer();

	// モデル取得
	PP::ppModel GetModel() { return model; }

	// ターゲットポジション設定
	void SetTargetPosition(DirectX::XMFLOAT3 position) { targetPosition = position; }

	// ターゲットポジション取得
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	// 攻撃範囲取得
	const float GetAttackRange() { return attackRange; }

	// 攻撃時のノードの半径
	const float GetAttackRadius() { return attackRadius; }

	// 実行タイマー設定
	void SetRunTimer(float timer) { runTimer = timer; }

	// 実行タイマー取得
	const float GetRunTimer() { return runTimer; }

	// リアクションフラグ設定
	void SetReaction(bool flag) { reactionFlag = flag; }

	// リアクションフラグ取得
	const bool GetReaction() { return reactionFlag; }

	// ダメージフラグ設定
	void SetDamageFlag(bool flag) { damageFlag = flag; }

	// ダメージフラグ取得
	const bool GetDamageFlag() { return damageFlag; }

	// 死亡フラグ設定
	void SetDeathFlag(bool flag) { deathFlag = flag; }

	// 死亡フラグ取得
	const bool GetDeathFlag() { return deathFlag; }

	// ロックオンされてるフラグ設定
	void SetLockedFlag(bool flag) { lockedFlag = flag; }

	// ロックオンされてるフラグ取得
	const bool GetLockedFlag() { return lockedFlag; }

	// 攻撃できるフラグ設定
	void SetAttackFlag(bool flag) { attackFlag = flag; }

	// 攻撃できるフラグ取得
	const bool GetAttackFlag() { return attackFlag; }

	// 攻撃したもしくはされたフラグ
	void SetAttackedFlag(bool flag) { attackedFlag = flag; }

	// 他の敵に呼ばれたフラグ設定
	void SetCalledFlag(bool flag) { calledFlag = flag; }

	// 他の敵に呼ばれたフラグ取得
	const bool GetCalledFlag() { return calledFlag; }

	// ボスに設定
	void SetBoss() { bossFlag = true; }

	// ボスかどうか
	const bool Checkboss() { return bossFlag; }

	// ID設定
	void SetId(int id) { this->id = id; }

	// ID取得
	int	GetId() { return id; }

	// ノードとプレイヤーの衝突処理
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
	float attackRadius = 0.2f; // 攻撃時に判定をとるノードの半径(初期値はスライムの攻撃半径)

	BehaviorTree* aiTree = nullptr;
	BehaviorData* behaviorData = nullptr;
	NodeBase* activeNode = nullptr;

	std::map<std::string, int> animationMap;

private:

	bool lockedFlag = false; // ロックオンされている
	bool reactionFlag = false;
	bool damageFlag = false;
	bool deathFlag = false;
	bool attackFlag = false;
	bool attackedFlag = false;
	bool calledFlag = false;
	bool bossFlag = false; // ボス敵かどうか

};



// 一旦ここに仮置き(後ほど別ヘッダーに移す)
// スライム
class Enemy01 : public Enemy
{
public:

	Enemy01();
	~Enemy01() override;

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	// メッセージ受信時の処理
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

