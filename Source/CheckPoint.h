#pragma once

#include "StageObject.h"

// �X�e�[�W
class CheckPoint : public StageObject
{
public:

	CheckPoint();
	~CheckPoint() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void RenderSub() override;

	void DebugRender() override;

private:

	PP::ppModel model = {};

	

};