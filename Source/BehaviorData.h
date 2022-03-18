#pragma once

#include <vector>
#include <stack>
#include <map>
#include "BehaviorTree.h"

// Behavior�ۑ��f�[�^
class BehaviorData
{
public:

	BehaviorData() { Init(); }

	// �V�[�P���X�m�[�h�̃v�b�V��
	void PushSequenceNode(NodeBase* node) { sequenceStack.push(node); }

	// �V�[�P���X�m�[�h�̃|�b�v
	NodeBase* PopSequenceNode();

	// �V�[�P���X�X�e�b�v�̃Q�b�^�[
	int GetSequenceStep(std::string name);

	// �V�[�P���X�X�e�b�v�̃Z�b�^�[
	void SetSequenceStep(std::string name, int step);

	// ����������
	void Init();

private:

	std::stack<NodeBase*> sequenceStack;			// �V�[�P���X�m�[�h�X�^�b�N
	std::map<std::string, int> runSequenceStepMap;	// ���s�V�[�P���X�̃X�e�b�v�}�b�v

};