#pragma once

#include <list>
#include "Lib/Lib.h"

class Effect
{
public:

	Effect() {}
	virtual ~Effect() {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(PP::RenderContext rc) = 0;

	void Destroy();

	PP::ppEffect pfc = nullptr;

};

class Effect01 : public Effect // Œã‚Ù‚Ç•Ê‚Ìcpp‚ÉˆÚ‚·
{
public:

	Effect01();
	~Effect01() override;

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

};