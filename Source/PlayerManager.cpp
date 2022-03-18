#include "PlayerManager.h"



void PlayerManager::Init()
{
	PP::Model::Load("Data/Model/Z_test/Paladin/Paladin.mdl", "PLAYER");
	player = new Player();
}

void PlayerManager::Update(float elapsedTime)
{
	player->Update(elapsedTime);
}

void PlayerManager::Render()
{
	player->Render();
}

void PlayerManager::RenderSub()
{
	player->RenderSub();
}

void PlayerManager::DebugRender()
{
	player->DebugRender();
}

void PlayerManager::Clear()
{
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
}