#include "EffectManager.h"



void Effect::Destroy()
{
	EffectManager::Instance().Remove(this);
}

Effect01::Effect01()
{
	pfc = EffectManager::Instance().GetPfc(EffectManager::Pfc::TEST);
	PP::Effect::Play(pfc, { 0, 0, 0 });
}

Effect01::~Effect01()
{

}

void Effect01::Update(float elapsedTime)
{
	PP::Effect::Update(pfc, elapsedTime);

	//if (PP::Input::CheckHitKey('E') == 1) PP::Effect::Play(pfc, { 0, 0, 0 });

	// Ä¶‚ªI‚í‚Á‚½‚çÁ‹?(Ä¶ŠÔ‚ğ•Ô‚·ŠÖ”‚ğ‚Â‚­‚é)

	//Destroy();
}

void Effect01::Render(PP::RenderContext rc)
{
	PP::Effect::Render(pfc, rc);
}