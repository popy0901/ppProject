#include "Shot.h"



//void Shot::Render(Object* obj)
//{
//	PP::Model::Render(obj->model, PP::Shader::RenderShadow());
//}
//
//void Shot::DebugRender(Object* obj)
//{
//	DirectX::XMFLOAT3 pos = { obj->direction.x, obj->direction.y, obj->direction.z };
//	pos.x += obj->wc.position.x;
//	pos.y += obj->wc.position.y;
//	pos.z += obj->wc.position.z;
//	PP::Debug::Primitive3D::DrawCylinder(pos, obj->radius, obj->height, { 0, 0, 0, 1 });
//}
//
//void ShotManager::Init()
//{
//	GetList()->clear();
//	PP::Model::Load("Data/Model/Sword/Sword.mdl", "SHOT");
//	//Add(Shot01::Instance(), "SHOT", { 0, 2, 0 }, {0, 0, 0});
//}
//
//Object* ShotManager::Add(MoveAlg* moveAlg, std::string id, 
//	const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 direction, const DirectX::XMFLOAT3 target)
//{
//	Object obj;
//	obj.moveAlg = moveAlg;
//	PP::Model::Make(obj.model, id);
//
//	obj.wc.position = position;
//	obj.direction = direction;
//	obj.target = target;
//
//	PP::Model::UpdateTransform(obj.model, obj.wc);
//	objList.emplace_back(obj);
//	return &(*objList.rbegin());
//}
//
//void Shot01::Update(Object* obj, float elapsedTime)
//{
//	switch (obj->state)
//	{
//	case 0:
//		obj->wc.scale = { 3.0f, 3.0f, 3.0f };
//		obj->radius = 0.5f;
//		obj->height = 0.1f;
//		obj->timer = 180;
//		obj->state++;
//		break;
//	case 1:
//
//		//obj->wc.position.x += obj->direction.x * speed;
//		//obj->wc.position.y += obj->direction.y * speed;
//		//obj->wc.position.z += obj->direction.z * speed;
//
//		obj->timer--;
//		if (obj->timer < 0) obj->Erase();
//
//		Move(obj, obj->direction, 10.0f);
//
//		// 指定位置(target)に向かって移動 // 後に関数化
//		{
//
//		}
//
//		// 進む向きを前にする // 後に関数化
//		{
//			DirectX::XMVECTOR Front, Up, Right;
//
//			// 前ベクトル算出
//			Front = DirectX::XMVectorSet(obj->direction.x, obj->direction.y, obj->direction.z, 0);
//			Front = DirectX::XMVector3Normalize(Front);
//			// 仮の上ベクトル算出
//			DirectX::XMVECTOR UP = DirectX::XMVectorSet(0, 1, 0, 0);
//
//			// 右ベクトル算出
//			Right = DirectX::XMVector3Cross(UP, Front);
//			Right = DirectX::XMVector3Normalize(Right);
//			// 上ベクトル算出
//			Up = DirectX::XMVector3Cross(Front, Right);
//			Up = DirectX::XMVector3Normalize(Up);
//			// 計算結果取り出し
//			DirectX::XMFLOAT3 right, up, front;
//			DirectX::XMStoreFloat3(&right, Right);
//			DirectX::XMStoreFloat3(&up, Up);
//			DirectX::XMStoreFloat3(&front, Front);
//
//			// 算出した軸ベクトルから行列を作成
//			transform._11 = right.x * obj->wc.scale.x;
//			transform._12 = right.y * obj->wc.scale.x;
//			transform._13 = right.z * obj->wc.scale.x;
//			transform._14 = 0.0f;
//			transform._21 = up.x * obj->wc.scale.y;
//			transform._22 = up.y * obj->wc.scale.y;
//			transform._23 = up.z * obj->wc.scale.y;
//			transform._24 = 0.0f;
//			transform._31 = front.x * obj->wc.scale.z;
//			transform._32 = front.y * obj->wc.scale.z;
//			transform._33 = front.z * obj->wc.scale.z;
//			transform._34 = 0.0f;
//			transform._41 = obj->wc.position.x;
//			transform._42 = obj->wc.position.y;
//			transform._43 = obj->wc.position.z;
//			transform._44 = 1.0f;
//
//			// 発射方向
//			obj->direction = front;
//		}
//
//		PP::Model::UpdateTransform(obj->model, transform);
//
//		//PP::Model::UpdateTransform(obj->model, obj->wc);
//
//		break;
//	}
//}