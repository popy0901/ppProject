#pragma once

#include "StageObject.h"


// �X�e�[�W�I�u�W�F�N�g�}�l�[�W���[
class StageObjectManager
{
public:

	StageObjectManager();
	~StageObjectManager();

	void Init();

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	void Register(StageObject* object);

	void Clear();

	static StageObjectManager& Instance()
	{
		return *instance;
	}

private:

	std::vector<StageObject*> objects;

	static StageObjectManager* instance;

};