#pragma once

#include "Enemy.h"

// �G�l�~�[�}�l�[�W��
class EnemyManager
{
private:

	EnemyManager() {}
	~EnemyManager() {}

public:

	static EnemyManager& Instance() // �V���O���g����ύX
	{
		static EnemyManager instance;
		return instance;
	}

	void Init();

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	void Register(Enemy* enemy, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& origin, const float range); // �o�^

	void Remove(Enemy* enemy); // �폜

	void Clear();

	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// ID����G�l�~�[���擾
	Enemy* GetEnemyFromId(int id);

private:

	// �G�l�~�[���m�̔��菈��
	void CollisionEnemyVsEnemies(); 

private:

	std::vector<Enemy*> enemies;
	std::vector<Enemy*> removes;

	int identity = 0; // �t�^����ID�̒l

};