#pragma once

#include "Ui.h"

class PlayerSp : public Ui
{
public:

	PlayerSp();
	~PlayerSp() override;

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void DebugRender();

};