#include "StageManager.h"
#include "StageMain.h"
//#include "StageMoveFloor.h"

void StageManager::Init()
{
	PP::Model::Load("Data/Model/Z_test/Stage/stage.mdl", "STAGE");
	PP::Model::Load("Data/Model/Z_test/Stage/CastleCollision.mdl", "COLLISIONSTAGE");
	PP::Model::Load("Data/Model/Z_test/Stage/stageNavi.mdl", "NAVISTAGE");
	//PP::Model::Load("Data/Model/Cube/Cube.mdl", "MOVESTAGE");
	Register(new StageMain());
	//Register(new StageMoveFloor());
}

void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages)
	{
		stage->Update(elapsedTime);
	}
}

void StageManager::Render()
{
	for (Stage* stage : stages)
	{
		stage->Render();
	}
}

void StageManager::RenderSub()
{
	for (Stage* stage : stages)
	{
		stage->RenderSub();
	}
}

void StageManager::DebugRender()
{
	for (Stage* stage : stages)
	{
		stage->DebugRender();
	}
}

void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

void StageManager::Clear()
{
	for (Stage* stage : stages)
	{
		if (stage != nullptr)
		{
			delete stage;
			stage = nullptr;
		}
	}
	stages.clear();
}

bool StageManager::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	PP::HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (Stage* stage : stages)
	{
		PP::HitResult hit2;
		if (stage->RayCast(start, end, hit2))
		{
			if (hit.distance > hit2.distance)
			{
				hit = hit2;
				result = true;
			}
		}
	}

	return result;
}