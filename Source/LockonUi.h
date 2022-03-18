#pragma once

#include "Ui.h"

class LockonUi : public Ui
{
public:

	LockonUi();
	~LockonUi() override;

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void DebugRender();

};