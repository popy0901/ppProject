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

	// �J�����h��
	if (shakeTimer > 0) // �h�ꎞ�Ԃ��ݒ肳�ꂽ��
	{
		// -1�`1
		newPosition.x += (PP::System::GetRandF(-1.0f, 1.0f)) * shakePower;
		newPosition.y += (PP::System::GetRandF(-1.0f, 1.0f)) * shakePower;
		newPosition.z += (PP::System::GetRandF(-1.0f, 1.0f)) * shakePower;
		shakeTimer -= elapsedTime;
	}

	// �n�`�Ƃ̂����蔻��(���C�L���X�g)
	PP::HitResult hit;
	if (StageManager::Instance().RayCast(newTarget, newPosition, hit))
	{
		float cPosY = newPosition.y;

		// �␳�O����␳��ւ̃x�N�g��
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

	// ���X�ɖڕW�ɋ߂Â���
	static constexpr float Speed = 1.0f / 8.0f;
	target.x += (newTarget.x - target.x) * Speed;
	target.y += (newTarget.y - target.y) * Speed;
	target.z += (newTarget.z - target.z) * Speed;
	position.x += (newPosition.x - position.x) * (Speed / 2);
	position.y += (newPosition.y - position.y) * (Speed / 2);
	position.z += (newPosition.z - position.z) * (Speed / 2);

	// �J�����̍��W�ƒ����_��ݒ�
	PP::Camera::SetLookAt(position, target, { 0, 1, 0 });
}

void Camera::TpsCamera(float elapsedTime)
{
	float ax = PP::Input::GetRightStickX(); // �E�X�e�B�b�NX
	float ay = PP::Input::GetRightStickY(); // �E�X�e�B�b�NY

	float speed = rollSpeed * elapsedTime;

	// ��]
	{
		angle.y += ax * speed;
		angle.x -= ay * speed;
	}

	// ��]����
	{
		if (angle.x < minAngleX) angle.x = minAngleX; // X��(�㉺��])
		if (angle.x > maxAngleX) angle.x = maxAngleX; // X��(�㉺��])
		if (angle.y < -DirectX::XM_PI) angle.y += DirectX::XM_2PI; // Y��(���E��])
		if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;  // Y��(���E��])
	}

	// �����_�̒���
	target.y += heightOffset;

	// �����_�������Ɉ�苗�����ꂽ�J�������W�����߂�
	if (ax != 0.0f || ay != 0.0f) // �蓮�ŃJ�����𓮂�������
	{
		// ��]�s����
		DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

		// �쐬�����s�񂩂�O���x�N�g�����Z�o
		DirectX::XMVECTOR Front = Transform.r[2];
		Front = DirectX::XMVector3Normalize(Front);
		DirectX::XMFLOAT3 front;
		DirectX::XMStoreFloat3(&front, Front);

		newPosition.x = target.x - front.x * range;
		newPosition.y = target.y - front.y * range + 1.0f;
		newPosition.z = target.z - front.z * range;
	}

	else // �����_(�v���C���[)��������������
	{
		// ���W���璍���_�̃x�N�g��
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
	// �w��ʒu(����΂Ɉړ�������)
	DirectX::XMVECTOR t0 = DirectX::XMVectorSet(targetWork[0].x, 0.5f, targetWork[0].z, 0.0f);
	DirectX::XMVECTOR t1 = DirectX::XMVectorSet(targetWork[1].x, 0.5f, targetWork[1].z, 0.0f);
	//DirectX::XMVECTOR crv = DirectX::XMLoadFloat3(&PP::Camera::GetRight());
	DirectX::XMVECTOR cuv = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR v = DirectX::XMVectorSubtract(t1, t0);
	DirectX::XMVECTOR l = DirectX::XMVector3Length(v);

	t0 = DirectX::XMLoadFloat3(&targetWork[0]); // �v���C���[�̍��W
	t1 = DirectX::XMLoadFloat3(&targetWork[1]); // �G�̍��W

	// �V���������_���Z�o(�v���C���[���W����G���W�̂���������)
	DirectX::XMVECTOR multiVec = DirectX::XMVectorSet(0.0f, 0.5f, 0.0f, 1.0f);
	DirectX::XMStoreFloat3(&newTarget, DirectX::XMVectorMultiplyAdd(v, multiVec, t0));

	if (heightOffset < 1.25f) heightOffset = 1.25f;
	newTarget.y += heightOffset; // �G���u�̍���*0.5�̒l�����Z

	// XMVectorClamp : V��Min����Max�܂ł̐��l��Ԃ�
	// XMVectorReplicate : XMVECTOR�̂S�̗v�f����Value�����ꂽ�l��Ԃ�

	// �V�������W�̎Z�o
	l = DirectX::XMVectorClamp(l, 
		DirectX::XMVectorReplicate(lengthLimit[0]), DirectX::XMVectorReplicate(lengthLimit[1]));

	t0 = DirectX::XMVectorMultiplyAdd(l, DirectX::XMVector3Normalize(DirectX::XMVectorNegate(v)), t0);
	//t0 = DirectX::XMVectorMultiplyAdd(crv, DirectX::XMVectorReplicate(sideValue * 3.0f), t0); // �Ȃ��Ă������H
	t0 = DirectX::XMVectorMultiplyAdd(cuv, DirectX::XMVectorReplicate(2.5f), t0); // ����

	DirectX::XMStoreFloat3(&newPosition, t0);
}

void Camera::EventCamera(float elapsedTime)
{

}

void Camera::OnTpsMode(DirectX::XMFLOAT3 target)
{
	if (mode != Mode::TpsCamera_)
	{
		// �p�x�Z�o
		DirectX::XMFLOAT3 v;
		v.x = { newPosition.x - newTarget.x };
		v.y = { newPosition.y - newTarget.y };
		v.z = { newPosition.z - newTarget.z };

		angle.y = atan2f(v.x, v.z) + DirectX::XM_PI;
		//angle.x = atan2f(v.y, v.z);

		// �p�x�̐��K��
		angle.y = atan2f(sinf(angle.y), cosf(angle.y));
		//angle.x = atan2f(sinf(angle.x), cosf(angle.y));
		angle.x = 0.0f;
	}

	mode = Mode::TpsCamera_;
	newTarget = target;
	heightOffset = 0.2f;
	//newTarget.y += 0.05f; // ��������
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
	// �O�ς�p���ĉ����̃Y�������Z�o
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
	// �^�[�Q�b�g�f�o�b�O
	PP::Debug::Primitive3D::DrawSphere(target, 0.1f, { 1, 1, 0, 1 });

	PP::Imgui::Begin(u8"CameraDebug", { 1600 - 128, 0 }, { 128, 256 });
	PP::Imgui::Text(u8"Position\n x:%2f\n y:%2f\n z:%2f", position.x, position.y, position.z);
	PP::Imgui::Text(u8"Angle\n x:%2f\n y:%2f\n z:%2f", angle.x, angle.y, angle.z);
	PP::Imgui::Text(u8"Target\n x:%2f\n y:%2f\n z:%2f", target.x, target.y, target.z);
	PP::Imgui::End();
}