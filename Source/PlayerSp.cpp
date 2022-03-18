#include "PlayerSp.h"
#include "PlayerManager.h"


PlayerSp::PlayerSp()
{
	position = { 64, SCREEN_HEIGHT - 160 };
	size = { 256, 16 }; // size.xÇ™SP?
	center = { 0, size.y / 2 };
	color = { 1.0f, 0.75f, 0.25f, 1.0f };
}

PlayerSp::~PlayerSp()
{
	Clear();
}

void PlayerSp::Update(float elapsedTime)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Update(elapsedTime);
	}

	Player* player = &PlayerManager::Instance().GetPlayer();

	const float stamina = static_cast<float>(player->GetStamina());
	const float maxStamina = static_cast<float>(player->GetMaxStamina());

	point = (stamina / maxStamina) * size.x;

}

void PlayerSp::Render(PP::RenderContext rc)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Render(rc);
	}

	const DirectX::XMFLOAT2 framePos = { position.x - 4, position.y };
	const DirectX::XMFLOAT2 frameSize = { size.x + 8, size.y + 4 };
	const DirectX::XMFLOAT2 frameCenter = { 0, frameSize.y / 2 };

	// ÉtÉåÅ[ÉÄ
	PP::Debug::Primitive2D::Box(framePos, frameSize, frameCenter, angle, { 1.0f, 1.0f, 1.0f, 1.0f });
	// 
	PP::Debug::Primitive2D::Box(position, size, center, angle, { 0.5f, 0.5f, 0.5f, 1.0f });
	// SP
	PP::Debug::Primitive2D::Box(position, { point, size.y }, center, angle, color);
}

void PlayerSp::DebugRender()
{

}