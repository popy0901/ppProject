#include "StageMain.h"



StageMain::StageMain()
{
	PP::Model::Make(model[0], "STAGE");
	PP::Model::Make(model[1], "COLLISIONSTAGE");
	PP::Model::Make(model[2], "NAVISTAGE");

	PP::WorldContext wc = {};
	wc.scale = { 0.01f, 0.01f, 0.01f }; // �K���ɒ���
	wc.angle = { 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f };
	wc.position = { 30.0f, 2.0f, 20.0f }; // �K���ɒ���
	PP::Model::UpdateTransform(model[0], wc);
	PP::Model::UpdateTransform(model[1], wc);
	PP::Model::UpdateTransform(model[2], wc);

	navi = new Navigation();
	navi->CreateStage(model[2]);
}

StageMain::~StageMain()
{
	//delete model;
	navi->Clear();
	delete navi;
}

void StageMain::Update(float elapsedTime)
{

}
static bool denote = 0; // ��X�����Ă���
void StageMain::Render()
{
	PP::Model::Render(model[denote], PP::Shader::RenderShadow());
}

void StageMain::RenderSub()
{
	PP::Model::Render(model[denote], PP::Shader::DepthShadow());
}

void StageMain::DebugRender()
{
	PP::Imgui::Begin(u8"�X�e�[�W", { 0, 256 }, { 128, 128 });
	PP::Imgui::CheckBox(u8"�\��", &denote);
	PP::Imgui::End();
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, PP::HitResult& hit)
{
	return PP::Collision::RayCast(model[1], start, end, hit);
}