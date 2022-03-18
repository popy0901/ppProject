#pragma once

#include <string>

class ActionBase;
class JudgementBase;
class NodeBase;
class BehaviorData;
class Enemy;

// ビヘイビアツリー
class BehaviorTree
{
public:

	// 選択ルール
	enum class SelectRule
	{
		None_,
		Priority_,			// 優先順位
		Sequence_,			// シーケンス(順序)
		SequentialLooping_, // シーケンシャルルーピング
		Random_,			// ランダム
	};

	BehaviorTree() : root(NULL) {}
	~BehaviorTree() { NodeAllClear(root); }

	// 実行ノードを推論する
	NodeBase* ActiveNodeInference(Enemy* enemy, BehaviorData* data);

	// シーケンスノードから推論開始
	NodeBase* SequenceBack(NodeBase* sequenceNode, Enemy* enemy, BehaviorData* data);

	// ノード追加
	void AddNode(std::string searchName, std::string entryName, 
		int priority, SelectRule selectRule, 
		JudgementBase* judgement, ActionBase* action);

	// 遷移した瞬間
	NodeBase* Transition(Enemy* enemy, NodeBase* activeNode, BehaviorData* data);

	// 実行
	NodeBase* Run(Enemy* enemy, NodeBase* actionNode, BehaviorData* data, float elapsedTime);

private:

	// ノード全消去
	void NodeAllClear(NodeBase* delNode);

	// ルートノード
	NodeBase* root;

};