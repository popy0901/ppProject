#pragma once

#include <vector>
#include "BehaviorTree.h"
#include "ActionBase.h"

// ���������[�N�����p
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

// �m�[�h
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

	// ���O�擾
	std::string GetName() { return name; }

	// �e�m�[�h�擾
	NodeBase* GetParent() { return parent; }

	// �q�m�[�h�擾
	NodeBase* GetChild(int index);

	// �q�m�[�h(����)�擾
	NodeBase* GetLastChild();

	// �q�m�[�h(�擪)�擾
	NodeBase* GetTopChild();

	// �Z��m�[�h�擾
	NodeBase* GetSibling() { return sibling; }

	// �K�w�ԍ��擾
	int GetHierarchyNo() { return hierarchyNo; }

	// �D�揇�ʎ擾
	int GetPriority() { return priority; }

	// �e�m�[�h�ݒ�
	void SetParent(NodeBase* parent) { this->parent = parent; }

	// �q�m�[�h�ǉ�
	void AddChild(NodeBase* child) { children.push_back(child); }

	// �Z��m�[�h�ݒ�
	void SetSibling(NodeBase* sibling) { this->sibling = sibling; }

	// �s���f�[�^�������Ă��邩
	bool HasAction() { return action != NULL ? true : false; }

	// ���s�۔���
	bool Judgement(Enemy* enemy);

	// �D�揇�ʑI��
	NodeBase* SelectPriority(std::vector<NodeBase*>* list);

	// �����_���I��
	NodeBase* SelectRandom(std::vector<NodeBase*>* list);

	// �V�[�P���X�I��
	NodeBase* SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data);

	// �m�[�h����
	NodeBase* SearchNode(std::string searchName);

	// �m�[�h���_
	NodeBase* Inference(Enemy* enemy, BehaviorData* data);

	// �J�ڂ����u��
	ActionBase::State Transition(Enemy* enemy);

	// ���s
	ActionBase::State Run(Enemy* enemy, float elapsedTime);

	std::vector<NodeBase*> children; // �q�m�[�h
protected:

	std::string					name;			// ���O
	BehaviorTree::SelectRule	selectRule;		// �I�����[��
	JudgementBase*				judgement;		// ����N���X
	ActionBase*					action;			// ���s�N���X
	unsigned int				priority;		// �D�揇��
	NodeBase*					parent;			// �e�m�[�h
	NodeBase*					sibling;		// �Z��m�[�h
	int							hierarchyNo;	// �K�w�ԍ�

};