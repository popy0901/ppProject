#include "EnemyHp.h"
#include "EnemyManager.h"


EnemyHp::EnemyHp()
{
	color = { 0.75f, 0.25f, 0.25f, 1.0f };
}

EnemyHp::~EnemyHp()
{
	Clear();
}

void EnemyHp::Update(float elapsedTime)
{
	for (Ui* ui : children)
	{
		if (ui != nullptr) ui->Update(elapsedTime);
	}
}

void EnemyHp::Render(PP::RenderContext rc)
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
		worldPos.y += enemy->GetHeight() * 1.25f;

		DirectX::XMVECTOR WorldPos = DirectX::XMLoadFloat3(&worldPos);

		DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project
			(WorldPos, 
			0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1,
			Projection, View, World);

		DirectX::XMFLOAT2 screenPos;
		DirectX::XMStoreFloat2(&screenPos, ScreenPos);

		const float gaugeW = enemy->GetRadius() * 200.0f;
		const float gaugeH = 5.0f;

		const float health = static_cast<float>(enemy->GetHealth())/* * static_cast<float>(enemy->GetMaxHealth())*/;
		const float maxHealth = static_cast<float>(enemy->GetMaxHealth())/* * static_cast<float>(enemy->GetMaxHealth())*/;

		// スクリーン変換した座標を基に表示位置の算出
		position = screenPos;
		size = { gaugeW, gaugeH };
		center = { size.x * 0.5f, size.y * 0.5f };

		// フレーム(外枠)のパラメータ
		const DirectX::XMFLOAT2 framePos = position;
		const DirectX::XMFLOAT2 frameSize = { size.x + 2, size.y + 2 };
		const DirectX::XMFLOAT2 frameCenter = { frameSize.x * 0.5f, frameSize.y * 0.5f };
		const DirectX::XMFLOAT4 frameColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		// 実際のHP残量(百分率で比率を演算)
		const DirectX::XMFLOAT2 hp = { (health / maxHealth) * gaugeW, gaugeH };

		// フレーム
		PP::Debug::Primitive2D::Box(framePos, frameSize, frameCenter, angle, frameColor);
		// HP
		PP::Debug::Primitive2D::Box(position, hp, center, angle, color);

	}
}

void EnemyHp::DebugRender()
{

}