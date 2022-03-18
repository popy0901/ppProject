#pragma once

#include "Effect.h"

class EffectManager
{
private:

	EffectManager() {}
	~EffectManager() {}

public:

	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	void Init();

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void Register(Effect* efc);

	void Remove(Effect* efc);

	void Clear();

	const PP::ppEffect& GetPfc(int Num) const { return pfces[Num]; }

	enum Pfc
	{
		TEST,
		END,
	};

private:

	std::vector<PP::ppEffect> pfces;

	std::vector<Effect*> efces;
	std::vector<Effect*> removes;

};