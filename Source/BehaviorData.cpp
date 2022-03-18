#include "BehaviorData.h"
#include "NodeBase.h"


NodeBase* BehaviorData::PopSequenceNode()
{
	// 空ならNULL
	if (sequenceStack.empty() != 0)
		return NULL;

	NodeBase* node = sequenceStack.top();
	if (node != NULL)
	{
		// 取り出したデータの削除
		sequenceStack.pop();
	}
	return node;
}

int BehaviorData::GetSequenceStep(std::string name)
{
	if (runSequenceStepMap.count(name) == 0)
		runSequenceStepMap.insert(std::make_pair(name, 0));

	return runSequenceStepMap.at(name);
}

void BehaviorData::SetSequenceStep(std::string name, int step)
{
	runSequenceStepMap.at(name) = step;
}

void BehaviorData::Init()
{
	runSequenceStepMap.clear();
	while (sequenceStack.size() > 0)
	{
		sequenceStack.pop();
	}
}