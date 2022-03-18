#pragma once

#include "JudgementBase.h"
#include "Enemy.h"

class WanderJudgement : public JudgementBase
{
public:

	WanderJudgement(Enemy* enemy) : JudgementBase(enemy) {}

	bool Judgement();

};

class BattleJudgement : public JudgementBase
{
public:

	BattleJudgement(Enemy* enemy) : JudgementBase(enemy) {}

	bool Judgement();

};

class AttackJudgement : public JudgementBase
{
public:

	AttackJudgement(Enemy* enemy) : JudgementBase(enemy) {}

	bool Judgement();

};

class DamageJudgement : public JudgementBase
{
public:

	DamageJudgement(Enemy* enemy) : JudgementBase(enemy) {}

	bool Judgement();

};

class DeathJudgement : public JudgementBase
{
public:

	DeathJudgement(Enemy* enemy) : JudgementBase(enemy) {}

	bool Judgement();

};