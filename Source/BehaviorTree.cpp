#include "BehaviorTree.h"
#include "ActionBase.h"
#include "NodeBase.h"
#include "JudgementBase.h"
#include "Enemy.h"
#include "BehaviorData.h"
#include "MetaAI.h"

NodeBase* BehaviorTree::ActiveNodeInference(Enemy* enemy, BehaviorData* data)
{
	// �f�[�^�����Z�b�g���ĊJ�n
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

	//// ����I��
	//if (state == ActionBase::State::Complete_)
	//{
	//	// �V�[�P���X�̓r�������f
	//	NodeBase* sequenceNode = data->PopSequenceNode();

	//	// �r������Ȃ��Ȃ�I��
	//	if (sequenceNode == NULL)
	//	{
	//		return NULL;
	//	}
	//	else // �r���Ȃ炻������n�߂�
	//		SequenceBack(sequenceNode, enemy, data);
	//}
	//// ���s�͏I��
	//else if (state == ActionBase::State::Failed_)
	//{
	//	return NULL;
	//}

	return actionNode;
}

NodeBase* BehaviorTree::Run(Enemy* enemy, NodeBase* actionNode, BehaviorData* data, float elapsedTime)
{
	// �m�[�h���s
	ActionBase::State state = actionNode->Run(enemy, elapsedTime);

	// TECK ���A�N�V�����p
	if (enemy->GetDamageFlag() || enemy->GetDeathFlag())
	{
		if (actionNode->GetName() != "Damage" && actionNode->GetName() != "Death")
		{
			return NULL;
		}
	}

	// ����I��
	if (state == ActionBase::State::Complete_)
	{
		// �V�[�P���X�̓r�����𔻒f
		NodeBase* sequenceNode = data->PopSequenceNode();

		// �r������Ȃ��Ȃ�I��
		if (sequenceNode == NULL)
		{
			return NULL;
		}
		else // �r���Ȃ炻������n�߂�
			return SequenceBack(sequenceNode, enemy, data);
	}
	// ���s�͏I��
	else if (state == ActionBase::State::Failed_)
		return NULL;

	// ����ێ�
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