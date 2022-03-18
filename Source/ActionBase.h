#pragma once

class Enemy;

// 行動処理基底クラス
class ActionBase
{
public:

	ActionBase(Enemy* enemy) : owner(enemy) {}
	
	// 実行処理
	enum class State
	{
		Run_, // 実行中
		Failed_, // 実行失敗
		Complete_, // 実行成功
	};

	// 遷移した瞬間の処理(純粋仮想関数)
	virtual ActionBase::State Transition() = 0;

	// 実行処理(純粋仮想関数)
	virtual ActionBase::State Run(float elapsedTime) = 0;

protected:

	Enemy* owner;

};