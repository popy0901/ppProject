#pragma once

#include "Enemy.h"

class Boss : public Enemy
{
public:

	Boss();
	~Boss() override;

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	// メッセージ受信時の処理
	bool OnMessage(const Telegram& telegram) override;

private:

	enum BossAnimation
	{
		ANIM_IDLE1,				// 待機
		ANIM_FLY_FORWARD,		// 空中前進
		ANIM_BASIC_ATTACK,		// かみつく
		ANIM_TAKE_OFF,			// 離陸
		ANIM_LAND,				// 着陸
		ANIM_SCREAM,			// 叫ぶ
		ANIM_CLAW_ATTACK,		// 爪攻撃
		ANIM_FLAME_ATTACK,		// ブレス
		ANIM_DEFEND,			// 守る
		ANIM_GET_HIT,			// 被弾
		ANIM_SLEEP,				// 眠る
		ANIM_WALK,				// 歩行
		ANIM_RUN,				// 走行
		ANIM_FLY_FLAME_ATTACK,	// 空中ブレス
		ANIM_ALY_GLIDE,			// 滑空
		ANIM_IDLE2,				// 待機2
		ANIM_DIE,				// 死亡
		ANIM_FLY_FLOAT,			// 浮遊
	};

};