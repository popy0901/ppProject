#pragma once

// ���M���郁�b�Z�[�W�̎��
enum class MESSAGE_TYPE : int
{
	MSG_CALL_HELP,					// ���Ԃɏ��������߂�
	MSG_CHANGE_ATTACK_AUTHORITY,	// �U������ς���(Enemy���烁�^AI)
	MSG_GIVE_ATTACK_AUTHORITY,		// �U������^����(���^AI����Enemy)
	MSG_SEEK_ATTACK_AUTHORITY,		// �U���������߂�(Enemy���烁�^AI)
};

class Telegram
{
public:

	int sender = 0;		// ���鑤��ID
	int receiver = 0;	// �󂯂鑤��ID
	MESSAGE_TYPE msg;	// ���b�Z�[�WNo

	// �R���X�g���N�^
	Telegram(int sender, int receiver, MESSAGE_TYPE msg)
		:sender(sender), receiver(receiver), msg(msg) {}

};