#pragma once

#include "Player.h"

// プレイヤーマネージャー
class PlayerManager
{
private:

	PlayerManager() {}
	~PlayerManager() {}

public:


	static PlayerManager& Instance()
	{
		static PlayerManager instance;
		return instance;
	}

	void Init();

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	void Clear();

	Player& GetPlayer() { return *player; }

private:

	Player* player = nullptr;

};