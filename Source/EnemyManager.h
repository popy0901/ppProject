#pragma once

#include "Enemy.h"

// エネミーマネージャ
class EnemyManager
{
private:

	EnemyManager() {}
	~EnemyManager() {}

public:

	static EnemyManager& Instance() // シングルトンを変更
	{
		static EnemyManager instance;
		return instance;
	}

	void Init();

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	void Register(Enemy* enemy, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& origin, const float range); // 登録

	void Remove(Enemy* enemy); // 削除

	void Clear();

	// エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// IDからエネミーを取得
	Enemy* GetEnemyFromId(int id);

private:

	// エネミー同士の判定処理
	void CollisionEnemyVsEnemies(); 

private:

	std::vector<Enemy*> enemies;
	std::vector<Enemy*> removes;

	int identity = 0; // 付与するIDの値

};