#pragma once

#include "Ui.h"

class PlayerHp : public Ui
{
public:

	PlayerHp();
	~PlayerHp() override;

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void DebugRender();

};