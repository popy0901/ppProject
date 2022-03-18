#include "Object.h"
#include "StageManager.h"


//void ObjectManager::Update(float elapsedTime)
//{
//	for (auto& it : objList)
//	{
//		if (it.moveAlg) it.moveAlg->Update(&it, elapsedTime);
//	}
//
//	auto it = objList.begin();
//	while (it != objList.end())
//	{
//		if (!it->moveAlg)it = objList.erase(it);
//		else it++;
//	}
//}
//
//void ObjectManager::Render()
//{
//	for (auto& it : objList)
//	{
//		if (it.moveAlg) it.moveAlg->Render(&it);
//	}
//}
//
//void ObjectManager::DebugRender()
//{
//	for (auto& it : objList)
//	{
//		if (it.moveAlg) it.moveAlg->DebugRender(&it);
//	}
//}
//
//void ObjectManager::RenderSub()
//{
//	for (auto& it : objList)
//	{
//		if (it.moveAlg) it.moveAlg->RenderSub(&it);
//	}
//}
//
//Object* ObjectManager::Add(MoveAlg* moveAlg, std::string id, const DirectX::XMFLOAT3& position)
//{
//	Object obj;
//	obj.moveAlg = moveAlg;
//	PP::Model::Make(obj.model, id);
//	
//	obj.wc.position = position;
//
//	PP::Model::UpdateTransform(obj.model, obj.wc);
//	objList.emplace_back(obj);
//	return &(*objList.rbegin());
//}

// ダメージを与える
bool Object::ApplyDamage(int damage, float invincibleTime)
{
	if (damage == 0) return false; // ダメージがない場合はfalse

	if (invincibleTimer > 0.0f) return false; // 無敵中のためfalse

	if (health <= 0) return false; // 体力がない場合はfalse

	health -= damage; // ダメージ処理
	invincibleTimer = invincibleTime;

	if (health <= 0) OnDead(); // 死亡通知
	else OnDamaged(); // 生きてるので　ダメージ通知

	return true; // ダメージがあった場合はtrue
}

// 衝撃を与える
void Object::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// 移動
void Object::Move(float vx, float vz, float speed)
{
	moveVecX = vx;
	moveVecZ = vz;

	maxSpeed = speed;
}

// 旋回
void Object::Turn(float vx, float vz, float speed, float elapsedTime)
{
	speed *= elapsedTime;
	
	// 進行ベクトルがゼロの場合は処理しない
	float length = sqrtf((vx * vx) + (vz * vz));
	if (length <= 0.0f) return;
	
	// 進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;
	
	// 自信の回転値から前方向を算出
	float frontX = sinf(wc.angle.y);
	float frontZ = cosf(wc.angle.y);
	
	// 回転角を求めるために2つの単位ベクトルの内積を算出
	float dot = (vx * frontX) + (vz * frontZ);
	
	// 内積値である-1.0～1.0から2つの単位ベクトルの角度が
	// 小さいほど1.0に近づくので、そこから回転速度を調整
	float rotation = 1.0f - dot;
	speed *= rotation;
	
	// 左右判定のために2つの単位ベクトルから内積を算出
	float cross = (vx * frontZ) - (vz * frontX);
	
	// 2Dの外積値が正か負かで左右判定が行う
	if (cross < 0.0f)
	{
		wc.angle.y -= speed; // 左回転
	}
	else
	{
		wc.angle.y += speed; // 右回転
	}
}

// ジャンプ
void Object::Jump(float speed)
{
	velocity.y = speed;
}

// 速力更新処理
void Object::UpdateVelocity(float elapsedTime)
{
	float elapsedFrame = 60.0f * elapsedTime;

	UpdateVerticalVelocity(elapsedFrame);

	UpdateVerticalMove(elapsedTime);

	UpdateHorizontalVelocity(elapsedFrame);

	UpdateHorizontalMove(elapsedTime);
}

// 無敵時間更新処理
void Object::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

// 垂直速力更新処理
void Object::UpdateVerticalVelocity(float elapsedFrame)
{
	// 重力処理
	velocity.y += gravity * elapsedFrame;
}

// 垂直移動更新処理
void Object::UpdateVerticalMove(float elapsedTime)
{
	// 垂直方向の移動量
	float my = velocity.y * elapsedTime;

	slopeRate = 0.0f;

	// キャラクターのY軸方向となる法線ベクトル
	//DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	// 落下中
	if (my < 0.0f)
	{
		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { wc.position.x , wc.position.y + stepOffset, wc.position.z };
		// レイの終了位置は移動後の位置
		DirectX::XMFLOAT3 end = { wc.position.x, wc.position.y + my + gravity, wc.position.z };

		// レイキャストによる地面判定
		PP::HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 法線ベクトル取得
			//normal = hit.normal;

			// 地面に接地している
			wc.position = hit.position;

			// 回転
			wc.angle.y += hit.rotation.y;

			// 傾斜率の計算
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// 着地した
			if (!isGround) OnLanding();
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			// 空中に浮いている
			if (wc.position.y > -1.75f) // 壁抜け時の落下対策(仮)
				wc.position.y += my;
			isGround = false;
		}

	}
	// 上昇中
	else if (my > 0.0f)
	{
		wc.position.y += my;
		isGround = false;
		//slopeRate = 0.0f;
	}

	// 地面の向きに沿うようにXZ軸回転
	/*{
		// Y軸が法線ベクトル方向に向くオイラー角回転を算出する
		wc.angle.x = atan2f(normal.z, normal.y);
		wc.angle.z = atan2f(normal.x, normal.y);
	
		float ax = atan2f(normal.z, normal.y);
		float az = atan2f(normal.x, normal.y);
	
		// 線形補完で滑らかに回転する
		wc.angle.x = PP::System::Lerp(wc.angle.x, ax, 0.2f);
		wc.angle.z = PP::System::Lerp(wc.angle.z, az, 0.2f);
	}*/
}

// 水平速力更新処理
void Object::UpdateHorizontalVelocity(float elapsedFrame)
{
	// XZ速力を減速する
	float length = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (length > 0.0f)
	{
		// 摩擦
		float friction = this->friction * elapsedFrame; 

		if (!IsGround()) friction *= airControl;

		// 摩擦による横方向の減速処理
		if (length > friction)
		{
			length -= friction;
			
			DirectX::XMFLOAT2 velo = { velocity.x, velocity.z };
			DirectX::XMVECTOR Velo = DirectX::XMLoadFloat2(&velo);
			Velo = DirectX::XMVector2Normalize(Velo);
			DirectX::XMStoreFloat2(&velo, Velo);
			velocity.x = velo.x * length;
			velocity.z = velo.y * length;

			// 単位ベクトル化
			//float vx = velocity.x / length;
			//float vz = velocity.z / length;
			//velocity.x = vx * friction;
			//velocity.z = vz * friction;
		}
		// 横方向の速力が摩擦以下になったので無効化
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	// XZ平面の速力を加速
	if (length <= maxSpeed)
	{
		// 移動ベクトルが0でないなら加速
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// 加速力
			float accel = this->accel * elapsedFrame;

			// 空中の場合加速力を減らす
			if (!IsGround()) accel *= airControl;

			// 移動ベクトルによる加速処理
			velocity.x += moveVecX * accel;
			velocity.z += moveVecZ * accel;

			// 最大速度制限
			float len = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
			if (len > maxSpeed)
			{
				//velocity.x = moveVecX * maxSpeed;
				//velocity.z = moveVecZ * maxSpeed;

				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxSpeed;
				velocity.z = vz * maxSpeed;
			}
			// 下り坂でガタらないようにする
			if (!isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * elapsedFrame; // めっちゃ細かくガタついてる？
			}
		}
	}
	// 移動ベクトルをリセット
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

// 水平移動更新処理
void Object::UpdateHorizontalMove(float elapsedTime)
{
	// 水平速力量計算
	float length = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (length > 0.0f)
	{
		// 水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { wc.position.x, wc.position.y + stepOffset, wc.position.z };
		DirectX::XMFLOAT3 end = { wc.position.x + mx, wc.position.y + stepOffset, wc.position.z + mz };

		// 壁判定(レイキャスト)
		PP::HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// 壁の法線
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			// 入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

			// 補正位置の計算
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			// 壁ずり方向へレイキャスト
			PP::HitResult hit2;
			if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2))
			{
				// 壁ずり方向で壁に当たらなかったら補正位置に移動
				wc.position.x = collectPosition.x;
				wc.position.z = collectPosition.z;
			}
			else
			{
				//wc.position.x = hit2.position.x;
				//wc.position.z = hit2.position.z;
			}
		}
		else
		{
			// 移動
			wc.position.x += mx;
			wc.position.z += mz;
		}
	}
}