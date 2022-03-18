#include "PlayerHp.h"
#include "PlayerManager.h"


PlayerHp::PlayerHp()
{
	position = { 64, SCREEN_HEIGHT - 128 };
	size = { 256, 32 }; // size.xがHP?
	center = { 0, size.y / 2 };
	color = { 0.25f, 1.0f, 0.25f, 1.0f };
}

PlayerHp::~PlayerHp()
{
	Clear();
}

void PlayerHp::Update(float elapsedTime)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Update(elapsedTime);
	}

	Player* player = &PlayerManager::Instance().GetPlayer();

	const float health = static_cast<float>(player->GetHealth());
	const float maxHealth = static_cast<float>(player->GetMaxHealth());

	point = (health / maxHealth) * size.x;

	// 親依存の処理を関数化?(今プロジェクトではたぶんいらない)

	/*if (parent != nullptr)
	{
		float parentPosX = parent->GetPosition().x;

		// position.x = parentPosX;

	}*/

}

void PlayerHp::Render(PP::RenderContext rc)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Render(rc);
	}

	const DirectX::XMFLOAT2 framePos = { position.x - 4, position.y };
	const DirectX::XMFLOAT2 frameSize = { size.x + 8, size.y + 4 };
	const DirectX::XMFLOAT2 frameCenter = { 0, frameSize.y / 2 };

	// フレーム
	PP::Debug::Primitive2D::Box(framePos, frameSize, frameCenter, angle, { 1.0f, 1.0f, 1.0f, 1.0f });
	// 
	PP::Debug::Primitive2D::Box(position, size, center, angle, { 0.5f, 0.5f, 0.5f, 1.0f });
	// HP
	PP::Debug::Primitive2D::Box(position, { point, size.y }, center, angle, color);

}

void PlayerHp::DebugRender()
{

}