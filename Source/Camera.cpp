#include "Camera.h"
#include "Lib/Lib.h"
#include "PlayerManager.h"
#include  "StageManager.h"

Camera* Camera::instance = nullptr;

Camera::Camera()
{
	instance = this;

	position = PP::Camera::GetPos();
	newPosition = PP::Camera::GetPos();
}

Camera::~Camera()
{

}

void Camera::Update(float elapsedTime)
{	
	switch (mode)
	{
	case Mode::TpsCamera_: TpsCamera(elapsedTime); break;
	case Mode::LockonCamera_: LockonCamera(elapsedTime); break;
	case Mode::MotionCamera_: EventCamera(elapsedTime); break;
	}

	// カメラ揺れ
	if (shakeTimer > 0) // 揺れ時間が設定されたら
	{
		// -1～1
		newPosition.x += (PP::System::GetRandF(-1.0f, 1.0f)) * shakePower;
		newPosition.y += (PP::System::GetRandF(-1.0f, 1.0f)) * shakePower;
		newPosition.z += (PP::System::GetRandF(-1.0f, 1.0f)) * shakePower;
		shakeTimer -= elapsedTime;
	}

	// 地形とのあたり判定(レイキャスト)
	PP::HitResult hit;
	if (StageManager::Instance().RayCast(newTarget, newPosition, hit))
	{
		float cPosY = newPosition.y;

		// 補正前から補正後へのベクトル
		DirectX::XMFLOAT3 v = 
		  { hit.position.x - newPosition.x, 
			hit.position.y - newPosition.y, 
			hit.position.z - newPosition.z };

		v.x *= 0.8f;
		v.y *= 0.8f;
		v.z *= 0.8f;

		newPosition.x += v.x;
		newPosition.y += v.y;
		newPosition.z += v.z;

		if (mode == Mode::TpsCamera_)
			newPosition.y = cPosY;
	}

	// 徐々に目標に近づける
	static constexpr float Speed = 1.0f / 8.0f;
	target.x += (newTarget.x - target.x) * Speed;
	target.y += (newTarget.y - target.y) * Speed;
	target.z += (newTarget.z - target.z) * Speed;
	position.x += (newPosition.x - position.x) * (Speed / 2);
	position.y += (newPosition.y - position.y) * (Speed / 2);
	position.z += (newPosition.z - position.z) * (Speed / 2);

	// カメラの座標と注視点を設定
	PP::Camera::SetLookAt(position, target, { 0, 1, 0 });
}

void Camera::TpsCamera(float elapsedTime)
{
	float ax = PP::Input::GetRightStickX(); // 右スティックX
	float ay = PP::Input::GetRightStickY(); // 右スティックY

	float speed = rollSpeed * elapsedTime;

	// 回転
	{
		angle.y += ax * speed;
		angle.x -= ay * speed;
	}

	// 回転制限
	{
		if (angle.x < minAngleX) angle.x = minAngleX; // X軸(上下回転)
		if (angle.x > maxAngleX) angle.x = maxAngleX; // X軸(上下回転)
		if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI; // Y軸(左右回転)
		if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;  // Y軸(左右回転)
	}

	// 注視点の調整
	target.y += heightOffset;

	// 注視点から後方に一定距離離れたカメラ座標を求める
	if (ax != 0.0f || ay != 0.0f) // 手動でカメラを動かしたら
	{
		// 回転行列作例
		DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

		// 作成した行列から前方ベクトルを算出
		DirectX::XMVECTOR Front = Transform.r[2];
		Front = DirectX::XMVector3Normalize(Front);
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);

		newPosition.x = target.x - front.x * range;
		newPosition.y = target.y - front.y * range + 1.0f;
		newPosition.z = target.z - front.z * range;
	}

	else // 注視点(プレイヤー)だけが動いたら
	{
		// 座標から注視点のベクトル
		DirectX::XMFLOAT3 v =
		{ target.x - position.x,
			target.y - position.y,
			target.z - position.z };

		DirectX::XMVECTOR V = DirectX::XMLoadFloat3(&v);
		V = DirectX::XMVector3Normalize(V);
		DirectX::XMStoreFloat3(&v, V);

		newPosition.x = target.x - v.x * range;
		//newPosition.y = target.y - v.y * range;
		newPosition.z = target.z - v.z * range;

		angle.y = atan2f(v.x, v.z);
	}

	if (newPosition.y < -1.5f) newPosition.y = -1.5f;
}

void Camera::LockonCamera(float elapsedTime)
{
	// 指定位置(後方斜に移動させる)
	DirectX::XMVECTOR t0 = DirectX::XMVectorSet(targetWork[0].x, 0.5f, targetWork[0].z, 0.0f);
	DirectX::XMVECTOR t1 = DirectX::XMVectorSet(targetWork[1].x, 0.5f, targetWork[1].z, 0.0f);
	//DirectX::XMVECTOR crv = DirectX::XMLoadFloat3(&PP::Camera::GetRight());
	DirectX::XMVECTOR cuv = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(t1, t0);
	DirectX::XMVECTOR l = DirectX::XMVector3Length(v);

	t0 = DirectX::XMLoadFloat3(&targetWork[0]); // プレイヤーの座標
	t1 = DirectX::XMLoadFloat3(&targetWork[1]); // 敵の座標

	// 新しい注視点を算出(プレイヤー座標から敵座標のだいたい間)
	DirectX::XMVECTOR multiVec = DirectX::XMVectorSet(0.0f, 0.5f, 0.0f, 1.0f);
	DirectX::XMStoreFloat3(&newTarget, DirectX::XMVectorMultiplyAdd(v, multiVec, t0));

	if (heightOffset < 1.25f) heightOffset = 1.25f;
	newTarget.y += heightOffset; // 敵モブの高さ*0.5の値を加算

	// XMVectorClamp : VのMinからMaxまでの数値を返す
	// XMVectorReplicate : XMVECTORの４つの要素数にValueをいれた値を返す

	// 新しい座標の算出
	l = DirectX::XMVectorClamp(l, 
		DirectX::XMVectorReplicate(lengthLimit[0]), DirectX::XMVectorReplicate(lengthLimit[1]));

	t0 = DirectX::XMVectorMultiplyAdd(l, DirectX::XMVector3Normalize(DirectX::XMVectorNegate(v)), t0);
	//t0 = DirectX::XMVectorMultiplyAdd(crv, DirectX::XMVectorReplicate(sideValue * 3.0f), t0); // なくてもいい？
	t0 = DirectX::XMVectorMultiplyAdd(cuv, DirectX::XMVectorReplicate(2.5f), t0); // 高さ

	DirectX::XMStoreFloat3(&newPosition, t0);
}

void Camera::EventCamera(float elapsedTime)
{

}

void Camera::OnTpsMode(DirectX::XMFLOAT3 target)
{
	if (mode != Mode::TpsCamera_)
	{
		// 角度算出
		DirectX::XMFLOAT3 v;
		v.x = { newPosition.x - newTarget.x };
		v.y = { newPosition.y - newTarget.y };
		v.z = { newPosition.z - newTarget.z };

		angle.y = atan2f(v.x, v.z) + DirectX::XM_PI;
		//angle.x = atan2f(v.y, v.z);

		// 角度の正規化
		angle.y = atan2f(sinf(angle.y), cosf(angle.y));
		//angle.x = atan2f(sinf(angle.x), cosf(angle.y));
		angle.x = 0.0f;
	}

	mode = Mode::TpsCamera_;
	newTarget = target;
	heightOffset = 0.2f;
	//newTarget.y += 0.05f; // 高さ調整
}

void Camera::OnLockonMode(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 target)
{
	if (mode != Mode::LockonCamera_)
	{
		sideValue = CalcSide(start, target);
	}

	mode = Mode::LockonCamera_;
	targetWork[0] = start;
	targetWork[1] = target;
	//targetWork[0].y += 0.01f;
	//targetWork[1].y += 0.01f;
}

void Camera::OnEventMode(void* data)
{

}

void Camera::OnShake(float timer, float power)
{
	shakeTimer = timer;
	shakePower = power;
}

float Camera::CalcSide(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	// 外積を用いて横軸のズレ方向算出
	float vx, vz;
	vx = position.x - target.x;
	vz = position.z - target.z;

	float l = sqrtf((vx * vx) + (vz * vz));

	vx /= l;
	vz /= l;

	float nx, nz;
	nx = p1.x - p2.x;
	nz = p1.z - p2.z;

	l = sqrtf((nx * nx) + (nz * nz));

	nx /= l;
	nz /= l;

	return ((vx * nz) - (vz * nx) < 0) ? + 1.0f : -1.0f;
}

void Camera::DebugRender()
{
	// ターゲットデバッグ
	PP::Debug::Primitive3D::DrawSphere(target, 0.1f, { 1, 1, 0, 1 });

	PP::Imgui::Begin(u8"CameraDebug", { 1600 - 128, 0 }, { 128, 256 });
	PP::Imgui::Text(u8"Position\n x:%2f\n y:%2f\n z:%2f", position.x, position.y, position.z);
	PP::Imgui::Text(u8"Angle\n x:%2f\n y:%2f\n z:%2f", angle.x, angle.y, angle.z);
	PP::Imgui::Text(u8"Target\n x:%2f\n y:%2f\n z:%2f", target.x, target.y, target.z);
	PP::Imgui::End();
}