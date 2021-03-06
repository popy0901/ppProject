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

	// 再生が終わったら消去?(再生時間を返す関数をつくる)

	//Destroy();
}

void Effect01::Render(PP::RenderContext rc)
{
	PP::Effect::Render(pfc, rc);
}