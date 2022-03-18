#include "LockonUi.h"
#include "EnemyManager.h"


LockonUi::LockonUi()
{
	color = { 0.9f, 0.9f, 0.9f, 1.0f };
}

LockonUi::~LockonUi()
{
	Clear();
}

void LockonUi::Update(float elapsedTime)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Update(elapsedTime);
	}
}

void LockonUi::Render(PP::RenderContext rc)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Render(rc);
	}

	// 一般エネミーはロックオン時、頭の上に表示(スクリーン変換)
	// ボスは固定位置に表示

	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);

	// 敵のHPを表示
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// ロックオンしていない敵はスルー
		if (!enemy->GetLockedFlag()) continue;

		// ワールド座標からスクリーン座標へ変換
		DirectX::XMFLOAT3 worldPos = enemy->GetPosition();
		worldPos.y += enemy->GetHeight() * 0.5f;

		DirectX::XMVECTOR WorldPos = DirectX::XMLoadFloat3(&worldPos);

		DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project
			(WorldPos, 
			0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1,
			Projection, View, World);

		DirectX::XMFLOAT2 screenPos;
		DirectX::XMStoreFloat2(&screenPos, ScreenPos);

		// スクリーン変換した座標を基に表示位置の算出
		position = screenPos;

		const float radius = 4.0f;

		PP::Debug::Primitive2D::Circle(position, radius, color);

	}
}

void LockonUi::DebugRender()
{

}