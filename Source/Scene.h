#pragma once

#include "Lib/Lib.h"

class Scene
{
public:

	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	
	virtual void Update(float elapsedTime) = 0;
	
	virtual void Render() = 0;
	
	virtual void DebugRender() = 0;
	
	virtual void End() = 0;

	bool IsFinish() const { return finished; }

	void SetFinish() { finished = true; }

private:

	bool finished = false; // Š®—¹ƒtƒ‰ƒO

protected:

	int timer = 0;
	int state = 0;

	PP::ppImage sprite = {};

};