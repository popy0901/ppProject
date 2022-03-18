#pragma once

#include <vector>
#include "BehaviorTree.h"
#include "ActionBase.h"

// メモリリーク調査用
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// ノード
class NodeBase
{
public:

	NodeBase(std::string name, NodeBase* parent, NodeBase* sibling,
		int priority, BehaviorTree::SelectRule selectRule, JudgementBase* judgement,
		ActionBase* action, int hierarchyNo):
		name(name), parent(parent), sibling(sibling),
	priority(priority), selectRule(selectRule), judgement(judgement),
	action(action), hierarchyNo(hierarchyNo), children(NULL){}

	~NodeBase();

	// 名前取得
	std::string GetName() { return name; }

	// 親ノード取得
	NodeBase* GetParent() { return parent; }

	// 子ノード取得
	NodeBase* GetChild(int index);

	// 子ノード(末尾)取得
	NodeBase* GetLastChild();

	// 子ノード(先頭)取得
	NodeBase* GetTopChild();

	// 兄弟ノード取得
	NodeBase* GetSibling() { return sibling; }

	// 階層番号取得
	int GetHierarchyNo() { return hierarchyNo; }

	// 優先順位取得
	int GetPriority() { return priority; }

	// 親ノード設定
	void SetParent(NodeBase* parent) { this->parent = parent; }

	// 子ノード追加
	void AddChild(NodeBase* child) { children.push_back(child); }

	// 兄弟ノード設定
	void SetSibling(NodeBase* sibling) { this->sibling = sibling; }

	// 行動データを持っているか
	bool HasAction() { return action != NULL ? true : false; }

	// 実行可否判定
	bool Judgement(Enemy* enemy);

	// 優先順位選択
	NodeBase* SelectPriority(std::vector<NodeBase*>* list);

	// ランダム選択
	NodeBase* SelectRandom(std::vector<NodeBase*>* list);

	// シーケンス選択
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data);

	// ノード検索
	NodeBase* SearchNode(std::string searchName);

	// ノード推論
	NodeBase* Inference(Enemy* enemy, BehaviorData* data);

	// 遷移した瞬間
	ActionBase::State Transition(Enemy* enemy);

	// 実行
	ActionBase::State Run(Enemy* enemy, float elapsedTime);

	std::vector<NodeBase*> children; // 子ノード
protected:

	std::string					name;			// 名前
	BehaviorTree::SelectRule	selectRule;		// 選択ルール
	JudgementBase*				judgement;		// 判定クラス
	ActionBase*					action;			// 実行クラス
	unsigned int				priority;		// 優先順位
	NodeBase*					parent;			// 親ノード
	NodeBase*					sibling;		// 兄弟ノード
	int							hierarchyNo;	// 階層番号

};