#include "CheckPoint.h"
#include "SceneManager.h" 
#include "PlayerManager.h"


CheckPoint::CheckPoint()
{
	PP::Model::Make(model, "CHECKPOINT");
	wc.scale = { 0.25f, 1.0f, 0.25f };
	wc.angle = { 0.0f, 0.0f, 0.0f };
	wc.position = { 35, 1, -20 };
	radius = 0.2f;
	height = 1.0f;
	PP::Model::UpdateTransform(model, wc);
}

CheckPoint::~CheckPoint()
{
	//delete model;
}

void CheckPoint::Update(float elapsedTime)
{
	Player* player = &PlayerManager::Instance().GetPlayer();
	wc.angle.y += 0.01f;

	// sin”g‚Å•‚—VŠ´‚ðo‚·
	{
		wc.position.y = sinf(wc.angle.y) * 0.2f;
	}

	// ƒvƒŒƒCƒ„[‚ªG‚ê‚½‚çƒNƒŠƒA‚ÖÝ’è
	{
		DirectX::XMFLOAT3 position = wc.position;
		position.y = wc.position.y - height * 0.5f;
		DirectX::XMFLOAT3 outPosition = {};
		if (PP::Collision::CylinderVsCylinder(
			position, radius, height,
			player->GetPosition(), player->GetRadius(), player->GetHeight(),
			outPosition))
		{
			SceneManager::Instance()->SetChangeType(SceneManager::CLEAR);
		}
	}

	PP::Model::UpdateTransform(model, wc);
	
}

void CheckPoint::Render()
{
	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void CheckPoint::RenderSub()
{
	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void CheckPoint::DebugRender()
{
	//DirectX::XMFLOAT3 position = wc.position;
	//position.y = wc.position.y - height * 0.5f;
	//PP::Debug::Primitive3D::DrawCylinder(position, radius, height, { 1, 1, 1, 1 });
}