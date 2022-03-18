#include "MetaAI.h"



Meta* Meta::instance = nullptr;

Meta::Meta(Player* pl, EnemyManager* enemyManager)
	: player(pl), enemyManager(enemyManager)
{
	instance = this;
}

void Meta::Update()
{
	// �����Ŏ����I�Ƀ��^AI���Ď����Ă��鏈��������΋L�ڂ���

}

void Meta::Discharge(Enemy* receiver, const Telegram& telegram)
{
	if (!receiver->OnMessage(telegram))
	{
		// ��M�ł��Ȃ������ꍇ
		_RPT0(_CRT_WARN, "\n error:Enemy Receive Failed");
	}
}

void Meta::Discharge(const Telegram& telegram)
{
	if (!OnMessage(telegram))
	{
		// ��M�ł��Ȃ������ꍇ
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
	case MESSAGE_TYPE::MSG_CALL_HELP: // ���̓G���Ă�
		for (int i = 0; i < enemyCount; ++i)
		{
			int enemyId = enemyManager->GetEnemy(i)->GetId();
			// ���M�҈ȊO�̃G�l�~�[�Ƀ��b�Z�[�W�𑗂�
			if (telegram.sender != enemyId)
			{
				// ���^AI����e�G�ɑ��郁�b�Z�[�W�� MSG_CALL_HELP
				SendMessaging(static_cast<int>(Meta::Identity::Meta), 
					enemyId, MESSAGE_TYPE::MSG_CALL_HELP);
			}
		}
		return true;
		break;
	case MESSAGE_TYPE::MSG_CHANGE_ATTACK_AUTHORITY: // �U�����̕ύX
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
			// �R���e�i���̃����_���ȓG�ɍU������^����
			int random = rand() % enemies.size();
			SendMessaging(static_cast<int>(Meta::Identity::Meta), 
				enemies[random]->GetId(), MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY);
		}
		return true;
		break;
	case MESSAGE_TYPE::MSG_SEEK_ATTACK_AUTHORITY: // �G����U�����̗v��
		int count = 0;
		for (int i = 0; i < enemyCount; ++i) // �U�����������Ă���G�̐����m�F
		{
			Enemy* enemy = enemyManager->GetEnemy(i);
			if (enemy->GetAttackFlag())count++;
		}
		// ���ɍU�����������Ă���G�����Ȃ��Ȃ狁�߂Ă����G�ɍU������^����
		if (count < 1)
		{
			// MetaAI����U���������߂Ă����G�ɍU������^����
			SendMessaging(static_cast<int>(Meta::Identity::Meta), telegram.sender, MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY);
		}
		return true;
		break;
	}

	return false;
}

void Meta::SendMessaging(int sender, int receiver, MESSAGE_TYPE msg)
{
	// MetaAI����
	if (receiver == static_cast<int>(Meta::Identity::Meta))
	{
		// ���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);

		// ���b�Z�[�W
		Discharge(telegram);
	}
	else // �G�l�~�[�B����M�҂̎��ǂ��ɑ��M���邩(��X�ύX����H)
	{
		// ��M�҂̃|�C���^���擾
		Enemy* receiveEnemy = enemyManager->GetEnemyFromId(receiver);

		// ��M�҂����Ȃ���ΕԂ�
		if (receiveEnemy == nullptr) return;

		// ���b�Z�[�W�f�[�^���쐬
		Telegram telegram(sender, receiver, msg);

		// ���b�Z�[�W
		Discharge(receiveEnemy, telegram);
	}
}