#pragma once

#include "Ui.h"

class Slot : public Ui
{
public:

	Slot();
	~Slot() override;

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void DebugRender();

private:

	PP::ppImage sprite[4] = {};

};