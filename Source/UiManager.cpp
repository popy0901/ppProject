#include "UiManager.h"
#include "PlayerHp.h"
#include "PlayerSp.h"
#include "SlotUi.h"
#include "EnemyHp.h"
#include "LockonUi.h"

UiManager* UiManager::instance = nullptr;

UiManager::UiManager()
{
	 instance = this; 
	 Init();
}

UiManager::~UiManager()
{
	Clear();
}

void UiManager::Init()
{
	PlayerHp* playerHp = nullptr;
	playerHp = new PlayerHp();
	Register(playerHp);

	PlayerSp* playerSp = nullptr;
	playerSp = new PlayerSp();
	Register(playerSp);

	Slot* slot = nullptr;
	slot = new Slot();
	Register(slot);

	EnemyHp* enemyHp = nullptr;
	enemyHp = new EnemyHp();
	Register(enemyHp);

	LockonUi* lockonUi = nullptr;
	lockonUi = new LockonUi();
	Register(lockonUi);

}

void UiManager::Update(float elapsedTime)
{
	for (Ui* ui : uies)
	{
		ui->Update(elapsedTime);
	}

	for (Ui* ui : removes)
	{
		std::vector<Ui*>::iterator it = std::find(uies.begin(), uies.end(), ui);

		if (it != uies.end())
		{
			uies.erase(it);
		}
		delete ui;
	}
	removes.clear();
}

void UiManager::Render(PP::RenderContext rc)
{
	for (Ui* ui : uies)
	{
		ui->Render(rc);
	}
}

void UiManager::DebugRender()
{
	for (Ui* ui : uies)
	{
		ui->DebugRender();
	}
}

void UiManager::Register(Ui* ui)
{
	uies.emplace_back(ui);
}

void UiManager::Remove(Ui* ui)
{
	removes.emplace_back(ui);
}

void UiManager::Clear()
{
	for (Ui* ui : uies)
	{
		if (ui != nullptr)
		{
			delete ui;
			ui = nullptr;
		}
	}
	uies.clear();
}