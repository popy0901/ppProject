
#include "JudgementBase.h"
#include "NodeBase.h"
#include "Enemy.h"
#include "BehaviorData.h"
#include "ActionBase.h"
#include "MetaAI.h"

NodeBase::~NodeBase()
{
	delete judgement;
	delete action;
}

NodeBase* NodeBase::GetChild(int index)
{
	if (children.size() > index)
		return NULL;

	return children.at(index);
}

NodeBase* NodeBase::GetLastChild()
{
	if (children.size() == 0)
		return NULL;

	return children.at(children.size() - 1);
}

NodeBase* NodeBase::GetTopChild()
{
	if (children.size() == 0)
		return NULL;

	return children.at(0);
}

NodeBase* NodeBase::SearchNode(std::string searchName)
{
	// ���O����v
	if (name == searchName)
	{
		return this;
	}

	else
	{
		// �q�m�[�h�Ō���
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			NodeBase* ret = (*itr)->SearchNode(searchName);

			if (ret != NULL)
			{
				return ret;
			}
		}
	}
	

	return NULL;
}

NodeBase* NodeBase::Inference(Enemy* enemy, BehaviorData* data)
{
	std::vector<NodeBase*> list;
	NodeBase* result = NULL;

	// �q�m�[�h�Ŏ��s�\�ȃm�[�h��T��
	for (int i = 0; i < children.size(); i++)
	{
		if (children.at(i)->judgement != NULL)
		{
			if (children.at(i)->judgement->Judgement() == true)
			{
				list.push_back(children.at(i));
			}
		}
		else // ����N���X���Ȃ���Ζ������ɒǉ�
			list.push_back(children.at(i));
	}

	// �I�����[���Ńm�[�h����
	switch (selectRule)
	{
		// �D�揇��
	case BehaviorTree::SelectRule::Priority_:
		result = SelectPriority(&list);
		break;
		// �����_��
	case BehaviorTree::SelectRule::Random_:
		result = SelectRandom(&list);
		break;
		// �V�[�P���X
	case BehaviorTree::SelectRule::Sequence_:
	case BehaviorTree::SelectRule::SequentialLooping_:
		result = SelectSequence(&list, data);
		break;
	}

	if (result != NULL)
	{
		// �s��������ΏI��
		if (result->HasAction() == true)
		{
			return result;
		}
		else // ���܂����m�[�h�Ő��_�J�n
			result = result->Inference(enemy, data);
	}

	return result;
}

NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* list)
{
	NodeBase* selectNode = NULL;
	int priority = INT_MAX;

	// �D�揇�ʂ̍����m�[�h��list����T��
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		if (priority > (*itr)->GetPriority())
		{
			selectNode = (*itr);
			priority = (*itr)->GetPriority();
		}
	}

	return selectNode;
}

NodeBase* NodeBase::SelectRandom(std::vector<NodeBase*>* list)
{
	// list�̃T�C�Y�ŗ��������߂�
	// ���߂��l��index�l�Ƃ��ăm�[�h���w�肷��R�[�h�����ōl���܂��傤
	int selectNo = rand() % list->size();
	return (*list).at(selectNo);
}

NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data)
{
	// ���̃m�[�h�̃V�[�P���X�̃X�e�b�v���擾
	int step = data->GetSequenceStep(name);

	// �V�[�P���X���I����Ă�����I��
	if (step >= children.size())
	{
		if (selectRule != BehaviorTree::SelectRule::SequentialLooping_)
		{
			return NULL;
		}
		else // Looping�̏ꍇ�͓�������s
			step = 0;
	}

	// ���Ԃ̃m�[�h�����s�ł��邩�̃`�F�b�N
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		if (children.at(step)->GetName() == (*itr)->GetName())
		{
			// �V�[�P���X�m�[�h���L�^
			data->PushSequenceNode(this);

			// �V�[�P���X�X�e�b�v���X�V
			data->SetSequenceStep(GetName(), step + 1);
			return children.at(step);
		}
	}

	return NULL;
}

bool NodeBase::Judgement(Enemy* enemy)
{
	// ����I�u�W�F�N�g�������Ă���Ύ��s���ĕԂ��Ă������ʂ�return
	if (judgement != NULL)
	{
		return judgement->Judgement();
	}
	// ����I�u�W�F�N�g���Ȃ����true
	return true;
}





ActionBase::State NodeBase::Transition(Enemy* enemy)
{
	// �A�N�V�����m�[�h������Ύ��s
	if (action != NULL)
	{
		return action->Transition();
	}
	return ActionBase::State::Failed_;
}

ActionBase::State NodeBase::Run(Enemy* enemy, float elapsedTime)
{
	// �A�N�V�����m�[�h������Ύ��s���Č��ʂ�Ԃ��A�Ȃ���Ύ��s����
	if (action != NULL)
	{
		return action->Run(elapsedTime);
	}

	return ActionBase::State::Failed_;
}