#pragma once

#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

// Behavior保存データ
class BehaviorData
{
public:

	BehaviorData() { Init(); }

	// シーケンスノードのプッシュ
	void PushSequenceNode(NodeBase* node) { sequenceStack.push(node); }

	// シーケンスノードのポップ
	NodeBase* PopSequenceNode();

	// シーケンスステップのゲッター
	int GetSequenceStep(std::string name);

	// シーケンスステップのセッター
	void SetSequenceStep(std::string name, int step);

	// 初期化処理
	void Init();

private:

	std::stack<NodeBase*> sequenceStack;			// シーケンスノードスタック
	std::map<std::string, int> runSequenceStepMap;	// 実行シーケンスのステップマップ

};