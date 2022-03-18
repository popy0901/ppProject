#pragma once

#include "ActionBase.h"
#include "Enemy.h"

// 徘徊行動
class WanderAction : public ActionBase
{
public:

	WanderAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// 待機行動
class IdleAction : public ActionBase
{
public:

	IdleAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// 追跡行動
class PursuitAction : public ActionBase
{
public:

	PursuitAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// 攻撃行動
class BodyAction : public ActionBase
{
public:

	BodyAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// ダメージ行動
class DamageAction : public ActionBase
{
public:

	DamageAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// 死亡行動
class DeathAction : public ActionBase
{
public:

	DeathAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};