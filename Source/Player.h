#pragma once

#include "Object.h"
#include "Enemy.h"
#include "Effect.h"

// プレイヤー
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

	// ガード時
	void OnGuarded() override;

	// ダメージ時
	void OnDamaged() override;

	// 死亡時
	void OnDead() override;

	// 着地した時に呼ばれる
	void OnLanding() override;

	// 入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	void UpdateHeal();

	void UpdateStamina();

	void UseStamina(float use, float timer); // 使用量、回復までの時間

	bool InputMove(float elapsedTime);

	bool InputWalk();

	bool InputDash();

	bool InputJump();

	bool InputAttack();

	bool InputGuard();

	bool InputHeal();

	bool InputDodge();

	bool FallCheck(); // 落下中か

	void InputProjectile();

	// カメラの状態を更新
	void UpdateCameraState(float elapsedTime);

	// ロックオン時のアニメーションを処理
	void LockonMoveAnimation(); 

	// 待機ステートへ遷移
	void TransIdleState();

	// 待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	// 歩行ステート遷移
	void TransWalkState();

	// 歩行ステート更新処理
	void UpdateWalkState(float elapsedTime);

	// 走行ステートへ遷移
	void TransRunState();

	// 走行ステート更新処理
	void UpdateRunState(float elapsedTime);

	// 走行(速い)ステートへ遷移
	void TransDashState();

	// 走行(速い)ステート更新処理
	void UpdateDashState(float elapsedTime);

	// ジャンプステートへ遷移
	void TransJumpState();

	// ジャンプステート更新処理
	void UpdateJumpState(float elapsedTime);

	// 落下ステートへ遷移
	void TransFallState();

	// 落下ステート更新処理
	void UpdateFallState(float elapedTime);

	// 着地ステート遷移
	void TransLandState();

	// 着地ステート更新処理
	void UpdateLandState(float elapsedTime);

	// 各攻撃の更新処理
	void AttackMoveUpdate(float elapsedTime);

	// 攻撃時の補正					補正受付時間の開始				補正受付時間の終了
	//				判定発生時間の開始				判定発生時間の終了
	//			先行入力可能時間
	void AttackCorrection(const float correctionTimeStart, const float correctionTimeEnd,
		const float collisionTimeStart, const float collisionTimeEnd, 
		const float advanceTime, float elapsedTime);

	// 攻撃1ステートへ遷移
	void TransAttack1State();

	// 攻撃1ステート更新処理
	void UpdateAttack1State(float elapsedTime);

	// 攻撃2ステートへ遷移
	void TransAttack2State();

	// 攻撃2ステート更新処理
	void UpdateAttack2State(float elapsedTime);

	// 攻撃3ステートへ遷移
	void TransAttack3State();

	// 攻撃3ステート更新処理
	void UpdateAttack3State(float elapsedTime);

	// 攻撃4ステートへ遷移
	void TransAttack4State();

	// 攻撃4ステート更新処理
	void UpdateAttack4State(float elapsedTime);

	// 回避ステートへ遷移
	void TransDodgeState();

	// 回避ステート更新処理
	void UpdateDodgeState(float elapsedTime);

	// ダメージステートへ遷移
	void TransDamageState();

	// ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	// 死亡ステートへ遷移
	void TransDeathState();
	void TransDeath2State();

	// 死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);

	// ガード待機ステートへ遷移
	void TransGuardIdleState();

	// ガード待機ステート更新処理
	void UpdateGuardIdleState(float elapsedTime);

	// ガード歩行ステートへ遷移
	void TransGuardWalkState();

	// ガード歩行ステート更新処理
	void UpdateGuardWalkState(float elapsedTime);

	// ガード走行ステートへ遷移
	void TransGuardRunState();

	// ガード走行ステート更新処理
	void UpdateGuardRunState(float elapsedTime);

	// ガード走行(速い)ステートへ遷移
	void TransGuardDashState();

	// ガード走行(速い)ステート更新処理
	void UpdateGuardDashState(float elapsedTime);

	// ガードリアクションステートへ遷移
	void TransGuardReactionState();

	// ガードリアクション更新処理
	void UpdateGuardReactionState(float elapsedTime);

	void UpdateGuard(); // ガード中の更新処理
	void DisGuard(); // ガード解除

	// プレイヤーとエネミーの衝突処理
	void CollisionPlayerVsEnemies();

	// 自機弾とエネミーの衝突処理
	void CollisionProjectileVsEnemies();

	// ノードとエネミーの衝突処理
	void CollisionAttackVsEnemies(const DirectX::XMFLOAT3& attackPosition, float nodeRadius);

	// ノードの座標を基準に別の座標を求める
	DirectX::XMFLOAT3 NodeToNewPosition(const char* nodeName, const DirectX::XMFLOAT3& position);

private:

	enum PlayerAnimation
	{
		
		ANIM_BACK_RUN,
		ANIM_BACK_WALK,
		ANIM_GUARD1, // 盾構え始め
		ANIM_GUARD2, // 盾構え中
		ANIM_GUARD3, // 盾受け時
		ANIM_DEATH1, // 死亡
		ANIM_DEATH2, // 死亡
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
		ANIM_RUN1, // 前
		ANIM_RUN2, // 前
		ANIM_WALK, // 前
		ANIM_FALL, // 落下中
		ANIM_LANDING1, // 着地
		ANIM_LANDING2,
		ANIM_DODGE,
		ANIM_ATTACK1, // 単発攻撃
		ANIM_ATTACK2, // 単発攻撃
		ANIM_ATTACK3, // 回転斬り
		ANIM_ATTACK4, // ジャンプ斬り(DA)
	};

	// ステート
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

	DirectX::XMFLOAT3 lockonDirection = {}; // 注視点の向き
	DirectX::XMFLOAT3 weaponPosition = {}; // 判定用の武器座標
	DirectX::XMFLOAT3 guardOffset = { 20, 30, 20 };// ガード時の角度調整

	DirectX::XMFLOAT2 attackDirection = {};
	DirectX::XMFLOAT2 dodgeDirection = {};

	float speed = 100.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 20.0f;

	// スタミナ関連
	float stamina = 0.0f;
	float maxStamina = 0.0f;
	float staminaTimer = 0.0f; // 回復し始めるまでの時間
	float staminaHeal = 0.25f;
	///////////////

	float nodeRadius = 0.5f; // ボーン判定用の半径

	float lockonTargetChangeTime = 0.0f; // 
	float lockonTargetChangeTimeMax = 8.0f; // 

	float fallTimer = 0.0f; // 落下時間

	int idleTimer = 0;

	int inputTimer = 0;

	int healItem = 3; // 回復アイテム数

	int runNum = 4; // ロックオン時のアニメーション分け

	int attackState = 0; // 攻撃動作の管理

	bool attackCollisionFlag = false;

	bool lockonFlag = false; // ロックオンカメラフラグ
	bool lockonButtonFlag = false;

	bool guardFlag = false; // ガードしている
	bool guardedFlag = false; // ガードしていた(モーション管理のため)

	bool cancelAttackFlag = false; // 動作キャンセルフラグ(攻撃へ遷移)
	bool cancelDodgeFlag = false; // 動作キャンセルフラグ(回避へ遷移)

	// Effect* hitEffect = nullptr;
	Effect* efc = nullptr;

	// ProjectileManager projectileManager;

};