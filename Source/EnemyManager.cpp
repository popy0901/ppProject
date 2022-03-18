#include "EnemyManager.h"
#include "Enemy.h"
#include "Boss.h"
#include "MetaAI.h"

void EnemyManager::Init()
{
	PP::Model::Load("Data/Model/Slime/Slime.mdl", "ENEMY1");
	PP::Model::Load("Data/Model/Z_test/MTKMDL/Whale/Whale.mdl", "ENEMY2");
	PP::Model::Load("Data/Model/Z_test/DragonModel/Boss.mdl", "BOSS");

	// TutoRoom
	Register(new Enemy01(), { 0, 0, 40 }, { -1, 0, 39 }, 6.0f);

	// NormalRoom
	//Register(new Enemy01(), { 55, 1, 32 }, { 55, 1, 32 }, 3.0f);
	//Register(new Enemy01(), { 60, 1, 40 }, { 60, 1, 40 }, 3.0f);
	//Register(new Enemy01(), { 43, 1, 44 }, { 43, 1, 44 }, 3.0f);
	//Register(new Enemy01(), { 47, 1, 31 }, { 47, 1, 31 }, 3.0f);

	// BossRoom
	Register(new Boss(), { 35, 1, -25 }, { 35, 1, -25 }, 3.0f);
}

void EnemyManager::Update(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

	// ”jŠüˆ—
	for (Enemy* enemy : removes)
	{
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		delete enemy;
	}
	removes.clear();

	// “G“¯m‚ÌÕ“Ë
	CollisionEnemyVsEnemies();
}

void EnemyManager::Render()
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render();
	}
}

void EnemyManager::RenderSub()
{
	for (Enemy* enemy : enemies)
	{
		enemy->RenderSub();
	}
}

void EnemyManager::DebugRender()
{
	for (Enemy* enemy : enemies)
	{
		enemy->DebugRender();
	}
}

void EnemyManager::Register(Enemy* enemy, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& origin, const float range)
{
	// ¶¬‚ÉID‚Ìİ’è
	enemy->SetId(identity + static_cast<int>(Meta::Identity::Enemy));
	identity++; // İ’è‚µ‚½‚Ì‚ÅƒCƒ“ƒNƒŠƒƒ“ƒg
	enemy->SetPosition(position);
	enemy->SetTerritory(origin, range);
	enemies.emplace_back(enemy);
}

void EnemyManager::Remove(Enemy* enemy)
{
	removes.emplace_back(enemy);
}

void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		if (enemy != nullptr)
		{
			delete enemy;
			enemy = nullptr;
		}
	}
	enemies.clear();
}

Enemy* EnemyManager::GetEnemyFromId(int id)
{
	for (Enemy* enemy : enemies)
	{
		if (enemy->GetId() == id)
			return enemy;
	}
	return nullptr;
}

void EnemyManager::CollisionEnemyVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// ‘S‚Ä‚Ì“G‚Æ‘“–‚½‚è‚ÅÕ“Ëˆ—
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy1 = enemyManager.GetEnemy(i);

		for (int j = i + 1; j < enemyCount; ++j)
		{
			Enemy* enemy2 = enemyManager.GetEnemy(j);

			// Õ“Ëˆ—
			DirectX::XMFLOAT3 outPosition;

			if (PP::Collision::CylinderVsCylinder(
				enemy1->GetPosition(), enemy1->GetRadius(), enemy1->GetHeight(),
				enemy2->GetPosition(), enemy2->GetRadius(), enemy2->GetHeight(),
				outPosition))
			{
				enemy2->SetPosition(outPosition);
			}

		}


	}
}