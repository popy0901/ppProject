#include "BehaviorTree.h"
#include "ActionBase.h"
#include "NodeBase.h"
#include "JudgementBase.h"
#include "Enemy.h"
#include "BehaviorData.h"
#include "MetaAI.h"

NodeBase* BehaviorTree::ActiveNodeInference(Enemy* enemy, BehaviorData* data)
{
	// データをリセットして開始
	data->Init();
	return root->Inference(enemy, data);
}

NodeBase* BehaviorTree::SequenceBack(NodeBase* sequenceNode, Enemy* enemy, BehaviorData* data)
{
	return sequenceNode->Inference(enemy, data);
}

void BehaviorTree::AddNode(std::string searchName, std::string entryName,
	int priority, SelectRule selectRule,
	JudgementBase* judgement, ActionBase* action)
{
	if (searchName != "")
	{
		NodeBase* searchNode = root->SearchNode(searchName);

		if (searchNode != NULL)
		{
			NodeBase* sibling = searchNode->GetLastChild();
			NodeBase* addNode = new NodeBase(entryName, searchNode, 
				sibling, priority, selectRule, judgement, 
				action, searchNode->GetHierarchyNo() + 1);

			searchNode->AddChild(addNode);
		}
	}

	else
	{
		if (root == NULL)
			root = new NodeBase(entryName, NULL, NULL,
				priority, selectRule, judgement, action, 1);
	}
}

NodeBase* BehaviorTree::Transition(Enemy* enemy, NodeBase* actionNode, BehaviorData* data)
{
	ActionBase::State state = actionNode->Transition(enemy);

	//// 正常終了
	//if (state == ActionBase::State::Complete_)
	//{
	//	// シーケンスの途中か判断
	//	NodeBase* sequenceNode = data->PopSequenceNode();

	//	// 途中じゃないなら終了
	//	if (sequenceNode == NULL)
	//	{
	//		return NULL;
	//	}
	//	else // 途中ならそこから始める
	//		SequenceBack(sequenceNode, enemy, data);
	//}
	//// 失敗は終了
	//else if (state == ActionBase::State::Failed_)
	//{
	//	return NULL;
	//}

	return actionNode;
}

NodeBase* BehaviorTree::Run(Enemy* enemy, NodeBase* actionNode, BehaviorData* data, float elapsedTime)
{
	// ノード実行
	ActionBase::State state = actionNode->Run(enemy, elapsedTime);

	// TECK リアクション用
	if (enemy->GetDamageFlag() || enemy->GetDeathFlag())
	{
		if (actionNode->GetName() != "Damage" && actionNode->GetName() != "Death")
		{
			return NULL;
		}
	}

	// 正常終了
	if (state == ActionBase::State::Complete_)
	{
		// シーケンスの途中かを判断
		NodeBase* sequenceNode = data->PopSequenceNode();

		// 途中じゃないなら終了
		if (sequenceNode == NULL)
		{
			return NULL;
		}
		else // 途中ならそこから始める
			return SequenceBack(sequenceNode, enemy, data);
	}
	// 失敗は終了
	else if (state == ActionBase::State::Failed_)
		return NULL;

	// 現状維持
	return actionNode;
}

void BehaviorTree::NodeAllClear(NodeBase* delNode)
{
	size_t count = delNode->children.size();
	if (count > 0)
	{
		for (NodeBase* node : delNode->children)
		{
			NodeAllClear(node);
		}
		delete delNode;
	}
	else
		delete delNode;
}