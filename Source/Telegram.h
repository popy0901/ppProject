#pragma once

// 送信するメッセージの種類
enum class MESSAGE_TYPE : int
{
	MSG_CALL_HELP,					// 仲間に助けを求める
	MSG_CHANGE_ATTACK_AUTHORITY,	// 攻撃権を変える(EnemyからメタAI)
	MSG_GIVE_ATTACK_AUTHORITY,		// 攻撃権を与える(メタAIからEnemy)
	MSG_SEEK_ATTACK_AUTHORITY,		// 攻撃権を求める(EnemyからメタAI)
};

class Telegram
{
public:

	int sender = 0;		// 送る側のID
	int receiver = 0;	// 受ける側のID
	MESSAGE_TYPE msg;	// メッセージNo

	// コンストラクタ
	Telegram(int sender, int receiver, MESSAGE_TYPE msg)
		:sender(sender), receiver(receiver), msg(msg) {}

};