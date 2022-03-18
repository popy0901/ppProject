#pragma once

class Enemy;

// �s���������N���X
class ActionBase
{
public:

	ActionBase(Enemy* enemy) : owner(enemy) {}
	
	// ���s����
	enum class State
	{
		Run_, // ���s��
		Failed_, // ���s���s
		Complete_, // ���s����
	};

	// �J�ڂ����u�Ԃ̏���(�������z�֐�)
	virtual ActionBase::State Transition() = 0;

	// ���s����(�������z�֐�)
	virtual ActionBase::State Run(float elapsedTime) = 0;

protected:

	Enemy* owner;

};