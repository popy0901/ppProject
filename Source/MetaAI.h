#pragma once

#include <set>
#include "Player.h"
#include "Telegram.h"
#include "EnemyManager.h"

class Meta
{
private:

	// Player��Enemy�Ƃ̒ʐM�@�\(Messaging)
	// std::set�͏����t�����ꂽ�f�[�^�𕡐��ێ����邱�Ƃ��ł��鏇���t�W��
	std::set<Telegram> priority;
	Player* player = nullptr;
	EnemyManager* enemyManager = nullptr;
	static Meta* instance;

public:

	enum class Identity : int
	{
		Meta = 0,
		Player = 1,	// 1�`4�̓v���C���[ID
		Enemy = 5,	// 5�ȍ~�̓G�l�~�[ID
	};

	Meta(Player* pl, EnemyManager* enemyManager);
	~Meta() {}

	// �C���X�^���X�擾
	static Meta& Instance() { return *instance; }

	// �X�V����
	void Update();

	// ���̃��\�b�h��Telegram�N���X���쐬����
	// Enemy���p������N���X�̃��b�Z�[�W���V�[�o�[�̏������Ăяo��
	void Discharge(Enemy* receiver, const Telegram& telegram);

	// MetaAI�̃��V�[�u�������w��
	void Discharge(const Telegram& telegram);

	// ���b�Z�[�W��M���̏���
	bool OnMessage(const Telegram& msg);

	// ���b�Z�[�W���M����
	void SendMessaging(int sender, int receiver, MESSAGE_TYPE msg);
};