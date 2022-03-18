#pragma once

#include "Object.h"

//class Shot : public MoveAlg
//{
//public:
//
//	void Render(Object* obj);
//
//	void DebugRender(Object* obj);
//
//protected:
//
//	DirectX::XMFLOAT4X4 transform = {};
//
//};
//
//class ShotManager : public ObjectManager
//{
//public:
//
//	void Init();
//
//	Object* Add(MoveAlg* moveAlg, std::string id, 
//		const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 direction, const DirectX::XMFLOAT3 target);
//
//	static ShotManager* Instance()
//	{
//		static ShotManager instance;
//		return &instance;
//	}
//
//private:
//
//
//
//};
//
//// ‰¼’u‚«
//class Shot01 : public Shot
//{
//public:
//
//	void Update(Object* obj, float elapsedTime);
//
//	static Shot01* Instance()
//	{
//		static Shot01 instance;
//		return &instance;
//	}
//
//private:
//
//
//
//};