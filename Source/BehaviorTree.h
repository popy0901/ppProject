#pragma once

#include <string>

class ActionBase;
class JudgementBase;
class NodeBase;
class BehaviorData;
class Enemy;

// �r�w�C�r�A�c���[
class BehaviorTree
{
public:

	// �I�����[��
	enum class SelectRule
	{
		None_,
		Priority_,			// �D�揇��
		Sequence_,			// �V�[�P���X(����)
		SequentialLooping_, // �V�[�P���V�������[�s���O
		Random_,			// �����_��
	};

	BehaviorTree() : root(NULL) {}
	~BehaviorTree() { NodeAllClear(root); }

	// ���s�m�[�h�𐄘_����
	NodeBase* ActiveNodeInference(Enemy* enemy, BehaviorData* data);

	// �V�[�P���X�m�[�h���琄�_�J�n
	NodeBase* SequenceBack(NodeBase* sequenceNode, Enemy* enemy, BehaviorData* data);

	// �m�[�h�ǉ�
	void AddNode(std::string searchName, std::string entryName, 
		int priority, SelectRule selectRule, 
		JudgementBase* judgement, ActionBase* action);

	// �J�ڂ����u��
	NodeBase* Transition(Enemy* enemy, NodeBase* activeNode, BehaviorData* data);

	// ���s
	NodeBase* Run(Enemy* enemy, NodeBase* actionNode, BehaviorData* data, float elapsedTime);

private:

	// �m�[�h�S����
	void NodeAllClear(NodeBase* delNode);

	// ���[�g�m�[�h
	NodeBase* root;

};