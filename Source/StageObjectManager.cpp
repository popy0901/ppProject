#include "StageObjectManager.h"
#include "CheckPoint.h"

StageObjectManager* StageObjectManager::instance = nullptr;

StageObjectManager::StageObjectManager()
{
	instance = this;
	Init();
}

StageObjectManager::~StageObjectManager()
{
	Clear();
}

void StageObjectManager::Init()
{
	PP::Model::Load("Data/Model/Cube/Cube.mdl", "CHECKPOINT");
	//Register(new CheckPoint());
}

void StageObjectManager::Update(float elapsedTime)
{
	for (StageObject* object : objects)
	{
		object->Update(elapsedTime);
	}
}

void StageObjectManager::Render()
{
	for (StageObject* object : objects)
	{
		object->Render();
	}
}

void StageObjectManager::RenderSub()
{
	for (StageObject* object : objects)
	{
		object->RenderSub();
	}
}

void StageObjectManager::DebugRender()
{
	for (StageObject* object : objects)
	{
		object->DebugRender();
	}
}

void StageObjectManager::Register(StageObject* stage)
{
	objects.emplace_back(stage);
}

void StageObjectManager::Clear()
{
	for (StageObject* object : objects)
	{
		if (object != nullptr)
		{
			delete object;
			object = nullptr;
		}
	}
	objects.clear();
}