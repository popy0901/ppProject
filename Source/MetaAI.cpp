#include "MetaAI.h"



Meta* Meta::instance = nullptr;

Meta::Meta(Player* pl, EnemyManager* enemyManager)
	: player(pl), enemyManager(enemyManager)
{
	instance = this;
}

void Meta::Update()
{
	// ここで自発的にメタAIが監視している処理があれば記載する

}

void Meta::Discharge(Enemy* receiver, const Telegram& telegram)
{
	if (!receiver->OnMessage(telegram))
	{
		// 受信できなかった場合
		_RPT0(_CRT_WARN, "\n error:Enemy Receive Failed");
	}
}

void Meta::Discharge(const Telegram& telegram)
{
	if (!OnMessage(telegram))
	{
		// 受信できなかった場合
		_RPT0(_CRT_WARN, "\n error:Meta Receive Failed");
	}
}

bool Meta::OnMessage(const Telegram& telegram)
{
	std::vector<Enemy*> enemies;
	enemies.clear();
	int enemyCount = enemyManager->GetEnemyCount();
	switch (telegram.msg)
	{
	case MESSAGE_TYPE::MSG_CALL_HELP: // 他の敵を呼ぶ
		for (int i = 0; i < enemyCount; ++i)
		{
			int enemyId = enemyManager->GetEnemy(i)->GetId();
			// 送信者以外のエネミーにメッセージを送る
			if (telegram.sender != enemyId)
			{
				// メタAIから各敵に送るメッセージも MSG_CALL_HELP
				SendMessaging(static_cast<int>(Meta::Identity::Meta), 
					enemyId, MESSAGE_TYPE::MSG_CALL_HELP);
			}
		}
		return true;
		break;
	case MESSAGE_TYPE::MSG_CHANGE_ATTACK_AUTHORITY: // 攻撃権の変更
		for (int i = 0; i < enemyCount; ++i)
		{
			Enemy* enemy = enemyManager->GetEnemy(i);
			DirectX::XMFLOAT3 position = enemy->GetPosition();
			DirectX::XMFLOAT3 targetPosition = player->GetPosition();

			float vx = targetPosition.x - position.x;
			float vz = targetPosition.z - position.z;
			float dist = sqrtf(vx * vx + vz * vz);

			if (dist < enemy->GetAttackRange())
			{
				enemies.push_back(enemy);
			}
		}

		if (enemies.size() > 0)
		{
			// コンテナ内のランダムな敵に攻撃権を与える
			int random = rand() % enemies.size();
			SendMessaging(static_cast<int>(Meta::Identity::Meta), 
				enemies[random]->GetId(), MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY);
		}
		return true;
		break;
	case MESSAGE_TYPE::MSG_SEEK_ATTACK_AUTHORITY: // 敵から攻撃権の要求
		int count = 0;
		for (int i = 0; i < enemyCount; ++i) // 攻撃権を持っている敵の数を確認
		{
			Enemy* enemy = enemyManager->GetEnemy(i);
			if (enemy->GetAttackFlag())count++;
		}
		// 他に攻撃権を持っている敵がいないなら求めてきた敵に攻撃権を与える
		if (count < 1)
		{
			// MetaAIから攻撃権を求めてきた敵に攻撃権を与える
			SendMessaging(static_cast<int>(Meta::Identity::Meta), telegram.sender, MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY);
		}
		return true;
		break;
	}

	return false;
}

void Meta::SendMessaging(int sender, int receiver, MESSAGE_TYPE msg)
{
	// MetaAI宛て
	if (receiver == static_cast<int>(Meta::Identity::Meta))
	{
		// メッセージデータを作成
		Telegram telegram(sender, receiver, msg);

		// メッセージ
		Discharge(telegram);
	}
	else // エネミー達が受信者の時どいつに送信するか(後々変更する？)
	{
		// 受信者のポインタを取得
		Enemy* receiveEnemy = enemyManager->GetEnemyFromId(receiver);

		// 受信者がいなければ返す
		if (receiveEnemy == nullptr) return;

		// メッセージデータを作成
		Telegram telegram(sender, receiver, msg);

		// メッセージ
		Discharge(receiveEnemy, telegram);
	}
}