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

	// ��ʃG�l�~�[�̓��b�N�I�����A���̏�ɕ\��(�X�N���[���ϊ�)
	// �{�X�͌Œ�ʒu�ɕ\��

	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&rc.view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&rc.projection);

	// �G��HP��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// ���b�N�I�����Ă��Ȃ��G�̓X���[
		if (!enemy->GetLockedFlag()) continue;

		// ���[���h���W����X�N���[�����W�֕ϊ�
		DirectX::XMFLOAT3 worldPos = enemy->GetPosition();
		worldPos.y += enemy->GetHeight() * 0.5f;

		DirectX::XMVECTOR WorldPos = DirectX::XMLoadFloat3(&worldPos);

		DirectX::XMVECTOR ScreenPos = DirectX::XMVector3Project
			(WorldPos, 
			0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1,
			Projection, View, World);

		DirectX::XMFLOAT2 screenPos;
		DirectX::XMStoreFloat2(&screenPos, ScreenPos);

		// �X�N���[���ϊ��������W����ɕ\���ʒu�̎Z�o
		position = screenPos;

		const float radius = 4.0f;

		PP::Debug::Primitive2D::Circle(position, radius, color);

	}
}

void LockonUi::DebugRender()
{

}