#include "EffectManager.h"



void EffectManager::Init()
{
	pfces.resize(Pfc::END);
	//PP::Effect::Load("Data/Effect/Pfc/test.pfc", pfces[Pfc::TEST]);
	//Register(new Effect01());
}

void EffectManager::Update(float elapsedTime)
{
	for (Effect* efc : efces)
	{
		efc->Update(elapsedTime);
	}

	for (Effect* efc : removes)
	{
		std::vector<Effect*>::iterator it = std::find(efces.begin(), efces.end(), efc);

		if (it != efces.end())
		{
			PP::Effect::Release(efc->pfc);
			efces.erase(it);
		}
		delete efc;
	}
	removes.clear();
}

void EffectManager::Render(PP::RenderContext rc)
{
	for (Effect* efc : efces)
	{
		efc->Render(rc);
	}
}

void EffectManager::Register(Effect* efc)
{
	efces.emplace_back(efc);
}

void EffectManager::Remove(Effect* efc)
{
	removes.emplace_back(efc);
}

void EffectManager::Clear()
{
	for (Effect* efc : efces)
	{
		if (efc != nullptr)
		{
			delete efc;
			efc = nullptr;
		}
	}
	efces.clear();
	pfces.clear();
}