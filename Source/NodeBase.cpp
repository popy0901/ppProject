
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
	// 名前が一致
	if (name == searchName)
	{
		return this;
	}

	else
	{
		// 子ノードで検索
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

	// 子ノードで実行可能なノードを探す
	for (int i = 0; i < children.size(); i++)
	{
		if (children.at(i)->judgement != NULL)
		{
			if (children.at(i)->judgement->Judgement() == true)
			{
				list.push_back(children.at(i));
			}
		}
		else // 判定クラスがなければ無条件に追加
			list.push_back(children.at(i));
	}

	// 選択ルールでノード決め
	switch (selectRule)
	{
		// 優先順位
	case BehaviorTree::SelectRule::Priority_:
		result = SelectPriority(&list);
		break;
		// ランダム
	case BehaviorTree::SelectRule::Random_:
		result = SelectRandom(&list);
		break;
		// シーケンス
	case BehaviorTree::SelectRule::Sequence_:
	case BehaviorTree::SelectRule::SequentialLooping_:
		result = SelectSequence(&list, data);
		break;
	}

	if (result != NULL)
	{
		// 行動があれば終了
		if (result->HasAction() == true)
		{
			return result;
		}
		else // 決まったノードで推論開始
			result = result->Inference(enemy, data);
	}

	return result;
}

NodeBase* NodeBase::SelectPriority(std::vector<NodeBase*>* list)
{
	NodeBase* selectNode = NULL;
	int priority = INT_MAX;

	// 優先順位の高いノードをlistから探す
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
	// listのサイズで乱数を求める
	// 求めた値をindex値としてノードを指定するコード自分で考えましょう
	int selectNo = rand() % list->size();
	return (*list).at(selectNo);
}

NodeBase* NodeBase::SelectSequence(std::vector<NodeBase*>* list, BehaviorData* data)
{
	// 今のノードのシーケンスのステップを取得
	int step = data->GetSequenceStep(name);

	// シーケンスが終わっていたら終了
	if (step >= children.size())
	{
		if (selectRule != BehaviorTree::SelectRule::SequentialLooping_)
		{
			return NULL;
		}
		else // Loopingの場合は頭から実行
			step = 0;
	}

	// 順番のノードが実行できるかのチェック
	for (auto itr = list->begin(); itr != list->end(); itr++)
	{
		if (children.at(step)->GetName() == (*itr)->GetName())
		{
			// シーケンスノードを記録
			data->PushSequenceNode(this);

			// シーケンスステップを更新
			data->SetSequenceStep(GetName(), step + 1);
			return children.at(step);
		}
	}

	return NULL;
}

bool NodeBase::Judgement(Enemy* enemy)
{
	// 判定オブジェクトを持っていれば実行して返ってきた結果をreturn
	if (judgement != NULL)
	{
		return judgement->Judgement();
	}
	// 判定オブジェクトがなければtrue
	return true;
}





ActionBase::State NodeBase::Transition(Enemy* enemy)
{
	// アクションノードがあれば実行
	if (action != NULL)
	{
		return action->Transition();
	}
	return ActionBase::State::Failed_;
}

ActionBase::State NodeBase::Run(Enemy* enemy, float elapsedTime)
{
	// アクションノードがあれば実行して結果を返す、なければ失敗扱い
	if (action != NULL)
	{
		return action->Run(elapsedTime);
	}

	return ActionBase::State::Failed_;
}