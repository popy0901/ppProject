#pragma once

#include "ActionBase.h"
#include "Enemy.h"

// �p�j�s��
class WanderAction : public ActionBase
{
public:

	WanderAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// �ҋ@�s��
class IdleAction : public ActionBase
{
public:

	IdleAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// �ǐՍs��
class PursuitAction : public ActionBase
{
public:

	PursuitAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// �U���s��
class BodyAction : public ActionBase
{
public:

	BodyAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// �_���[�W�s��
class DamageAction : public ActionBase
{
public:

	DamageAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};

// ���S�s��
class DeathAction : public ActionBase
{
public:

	DeathAction(Enemy* enemy) : ActionBase(enemy) {}

	ActionBase::State Transition();

	ActionBase::State Run(float elapsedTime);

};