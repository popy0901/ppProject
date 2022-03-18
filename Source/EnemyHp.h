#pragma once

#include "Ui.h"

class EnemyHp : public Ui
{
public:

	EnemyHp();
	~EnemyHp() override;

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void DebugRender();

};