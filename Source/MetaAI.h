#pragma once

#include <set>
#include "Player.h"
#include "Telegram.h"
#include "EnemyManager.h"

class Meta
{
private:

	// PlayerやEnemyとの通信機能(Messaging)
	// std::setは順序付けされたデータを複数保持することができる順序付集合
	std::set<Telegram> priority;
	Player* player = nullptr;
	EnemyManager* enemyManager = nullptr;
	static Meta* instance;

public:

	enum class Identity : int
	{
		Meta = 0,
		Player = 1,	// 1～4はプレイヤーID
		Enemy = 5,	// 5以降はエネミーID
	};

	Meta(Player* pl, EnemyManager* enemyManager);
	~Meta() {}

	// インスタンス取得
	static Meta& Instance() { return *instance; }

	// 更新処理
	void Update();

	// このメソッドはTelegramクラスを作成して
	// Enemyを継承するクラスのメッセージレシーバーの処理を呼び出す
	void Discharge(Enemy* receiver, const Telegram& telegram);

	// MetaAIのレシーブ処理を指定
	void Discharge(const Telegram& telegram);

	// メッセージ受信時の処理
	bool OnMessage(const Telegram& msg);

	// メッセージ送信処理
	void SendMessaging(int sender, int receiver, MESSAGE_TYPE msg);
};