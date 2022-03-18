#pragma once

#include "Lib/Lib.h"

// ��
class Sky
{
public:

	Sky();
	~Sky();

	void Update(float elapsedTime);

	void Render();

	void DebugRender();

private:

	PP::ppModel model = nullptr;

};