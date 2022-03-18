#include "Sky.h"



Sky::Sky()
{
	PP::Model::Make(model, "SKY");

	PP::WorldContext wc = {};
	wc.scale = { 5.0f, 5.0f, 5.0f };
	PP::Model::UpdateTransform(model, wc);
}

Sky::~Sky()
{
	// delete model;
}

void Sky::Update(float elapsedTime)
{

}

void Sky::Render()
{
	PP::Model::Render(model, PP::Shader::Lambert());
}

void Sky::DebugRender()
{

}
