#pragma once

#include "Enemy.h"

class Boss : public Enemy
{
public:

	Boss();
	~Boss() override;

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	// ���b�Z�[�W��M���̏���
	bool OnMessage(const Telegram& telegram) override;

private:

	enum BossAnimation
	{
		ANIM_IDLE1,				// �ҋ@
		ANIM_FLY_FORWARD,		// �󒆑O�i
		ANIM_BASIC_ATTACK,		// ���݂�
		ANIM_TAKE_OFF,			// ����
		ANIM_LAND,				// ����
		ANIM_SCREAM,			// ����
		ANIM_CLAW_ATTACK,		// �܍U��
		ANIM_FLAME_ATTACK,		// �u���X
		ANIM_DEFEND,			// ���
		ANIM_GET_HIT,			// ��e
		ANIM_SLEEP,				// ����
		ANIM_WALK,				// ���s
		ANIM_RUN,				// ���s
		ANIM_FLY_FLAME_ATTACK,	// �󒆃u���X
		ANIM_ALY_GLIDE,			// ����
		ANIM_IDLE2,				// �ҋ@2
		ANIM_DIE,				// ���S
		ANIM_FLY_FLOAT,			// ���V
	};

};