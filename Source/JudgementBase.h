#pragma once

class Enemy;

// Às”»’è
class JudgementBase
{
public:

	JudgementBase(Enemy* enemy) : owner(enemy) {}

	virtual bool Judgement() = 0;

protected:

	Enemy* owner;

};