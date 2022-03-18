#include "SlotUi.h"
#include "PlayerManager.h"


Slot::Slot()
{
	PP::Image::Load("Data/Image/Potion.png", sprite[0]);
	PP::Image::Load("Data/Image/01.png", sprite[1]);
	PP::Image::Load("Data/Image/02.png", sprite[2]);
	PP::Image::Load("Data/Image/03.png", sprite[3]);

	position = { 128, SCREEN_HEIGHT - 256 };
	size = { 128, 128 }; // size.xがHP?
	center = { size.x / 2, size.y / 2 };
	color = { 1.0f, 1.0f, 1.0f, 1.0f };
	angle = 0.0f;
}

Slot::~Slot()
{
	Clear();
}

void Slot::Update(float elapsedTime)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Update(elapsedTime);
	}
}

void Slot::Render(PP::RenderContext rc)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Render(rc);
	}

	const DirectX::XMFLOAT2 framePos = { position.x, position.y };
	const DirectX::XMFLOAT2 frameSize = { size.x + 4, size.y + 4 };
	const DirectX::XMFLOAT2 frameCenter = { frameSize.x / 2, frameSize.y / 2 };

	Player* player = &PlayerManager::Instance().GetPlayer();
	int itemNum = player->GetHealItem();

	PP::Debug::Primitive2D::Box(framePos, frameSize, frameCenter, angle, { 0.75f, 0.75f, 0.75f, 1.0f });
	PP::Debug::Primitive2D::Box(position, size, center, angle, { 0.25f, 0.25f, 0.25f, 1.0f });

	// アイコン表示
	PP::Image::Render(position,
		{ 0.5f, 0.5f },
		angle, color, sprite[0]);

	// 残量表示
	if (itemNum > 0 && itemNum < 4)
	{
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
		PP::Image::Render({ position.x + size.x / 3, position.y + size.y / 3 },
			{ 0.5f, 0.5f },
			angle, color, sprite[itemNum]);
	}
	else color = { 0.5f, 0.5f, 0.5f, 1.0f };
}

void Slot::DebugRender()
{

}