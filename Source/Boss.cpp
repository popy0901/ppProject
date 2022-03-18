#include "Boss.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "NodeBase.h"
#include "JudgementOwner.h"
#include "ActionOwner.h"

Boss::Boss()
{
	PP::Model::Make(model, "BOSS");

	{
		animationMap.insert(std::make_pair("IDLE", ANIM_IDLE1)); // 待機
		animationMap.insert(std::make_pair("BATTLE_IDLE", ANIM_IDLE2)); // 戦闘待機
		animationMap.insert(std::make_pair("ATTACK1", ANIM_BASIC_ATTACK)); // かみつく
		animationMap.insert(std::make_pair("WALK", ANIM_WALK)); // 歩行
		animationMap.insert(std::make_pair("RUN", ANIM_RUN)); // 走行
		animationMap.insert(std::make_pair("HIT", ANIM_GET_HIT)); // 被弾
		animationMap.insert(std::make_pair("DEATH", ANIM_DIE)); // 死亡
	}

	SetScale({ 0.01f, 0.01f, 0.01f });
	SetAngle({ 0.0f, DirectX::XMConvertToRadians(0.0f), 0.0f });
	SetRadius(3.0f);
	SetHeight(4.0f);

	PP::Model::UpdateTransform(model, wc);

	SetBoss();

	attackRadius = 1.0f;
	attackRange = 5.0f;
	searchRange = 20.0f;
	health = 5;
	maxHealth = health;

	// ビヘイビアツリー設定
	behaviorData = new BehaviorData();
	aiTree = new BehaviorTree();

	// 構築
	{
		aiTree->AddNode("", "Root", 0, BehaviorTree::SelectRule::Priority_, NULL, NULL);
		aiTree->AddNode("Root", "Death", 1, BehaviorTree::SelectRule::None_, new DeathJudgement(this), new DeathAction(this));
		aiTree->AddNode("Root", "Damage", 2, BehaviorTree::SelectRule::None_, new DamageJudgement(this), new DamageAction(this));
		aiTree->AddNode("Root", "Battle", 3, BehaviorTree::SelectRule::Priority_, new BattleJudgement(this), NULL);
		aiTree->AddNode("Root", "Scount", 4, BehaviorTree::SelectRule::Priority_, NULL, NULL);
		aiTree->AddNode("Battle", "Attack", 1, BehaviorTree::SelectRule::Random_, new AttackJudgement(this), NULL);
		//aiTree->AddNode("Battle", "Turn", 2, BehaviorTree::SelectRule::None_, NULL, new TurnAction(this));
		aiTree->AddNode("Battle", "Pursuit", 3, BehaviorTree::SelectRule::None_, NULL, new PursuitAction(this));
		// AttackNodeから条件状態を見て攻撃パターンを変更できるといいかも？
		// Judgementを増やす？
		aiTree->AddNode("Attack", "Body", 0, BehaviorTree::SelectRule::None_, NULL, new BodyAction(this));
		//aiTree->AddNode("Scount", "Wander", 1, BehaviorTree::SelectRule::None_, new WanderJudgement(this), new WanderAction(this));
		aiTree->AddNode("Scount", "Idle", 1, BehaviorTree::SelectRule::None_, NULL, new IdleAction(this));
	}

}

Boss::~Boss()
{
	// delete model;
	delete aiTree;
	delete behaviorData;
}

void Boss::Update(float elapsedTime)
{
	// 現在実行中のノードがあるか
	if (activeNode == nullptr)
	{
		// なければ実行するノードを推論し、activeNodeに格納
		activeNode = aiTree->ActiveNodeInference(this, behaviorData);
	}

	// 再度ノードが実行中か判断
	if (activeNode != nullptr)
	{
		if (GetReaction()) SetRunTimer(0.0f);
		// 指定のactiveNodj,eをaiTreeから実行
		// 戻り値でactiveNodeを更新
		if (GetRunTimer() == 0.0f)
			activeNode = aiTree->Transition(this, activeNode, behaviorData);

		activeNode = aiTree->Run(this, activeNode, behaviorData, elapsedTime);
	}

	SetLockedFlag(false);

	UpdateVelocity(elapsedTime);

	UpdateInvincibleTimer(elapsedTime);

	PP::Model::UpdateAnimation(model, elapsedTime);

	PP::Model::UpdateTransform(model, wc);
}

void Boss::Render()
{
	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void Boss::RenderSub()
{
	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void Boss::DebugRender()
{
	PP::Debug::Primitive3D::DrawCylinder(wc.position, radius, height, { 0, 0, 0, 1 });

	PP::Debug::Primitive3D::DrawCylinder(wc.position, attackRange, 1.0f, { 1, 0, 1, 1 });
}

bool Boss::OnMessage(const Telegram& telegram)
{
	switch (telegram.msg)
	{
	case MESSAGE_TYPE::MSG_CALL_HELP:
		return true;
		break;
	case MESSAGE_TYPE::MSG_GIVE_ATTACK_AUTHORITY:
			SetAttackFlag(true);
			return true;
			break;
	}
	return false;
}