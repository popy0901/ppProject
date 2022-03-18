#include "StageMoveFloor.h"



StageMoveFloor::StageMoveFloor()
{
	PP::Model::Make(model, "MOVESTAGE");
	scale = { 3.0f, 0.5f, 3.0f };
	SetStartPoint({ 0.0f, 1.0f, 2.0f });
	SetGoalPoint({ 10.0f, 2.0f, 2.0f });
	SetTorque({ 0.0f, 1.0f, 0.0f });
	UpdateTransform();
	PP::Model::UpdateTransform(model, transform);
}

StageMoveFloor::~StageMoveFloor()
{
	//delete model;
}

void StageMoveFloor::Update(float elapsedTime)
{
	// �O��̏���ۑ�
	oldTransform = transform;
	oldAngle = angle;

	// �X�^�[�g����S�[���܂ł̋������Z�o
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR Goal = DirectX::XMLoadFloat3(&goal);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Goal, Start);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);
	float length;
	DirectX::XMStoreFloat(&length, Length);

	// �X�^�[�g����S�[���܂ł̊Ԃ���b�ԂŐi�ފ���(0.0�`1.0)���Z�o
	float speed = moveSpeed * elapsedTime;
	float speedRate = speed / length;
	moveRate += speedRate;

	// �S�[���ɓ��B�A�܂��̓X�^�[�g�ɖ߂����ꍇ�A�ړ������𔽓]������
	if (moveRate <= 0.0f || moveRate >= 1.0f)
	{
		moveSpeed = -moveSpeed;
	}

	// ���`�⊮�ňʒu���Z�o
	DirectX::XMVECTOR Position = DirectX::XMVectorLerp(Start, Goal, moveRate);
	DirectX::XMStoreFloat3(&position, Position);

	// ��]
	angle.x += torque.x * elapsedTime;
	angle.y += torque.y * elapsedTime;
	angle.z += torque.z * elapsedTime;

	UpdateTransform();

	// ���C�L���X�g�悤�Ƀ��f����ԍs��ɂ��邽�ߒP�ʍs���n��
	const DirectX::XMFLOAT4X4 transformIdentity = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	PP::Model::UpdateTransform(model, transformIdentity);

}

void StageMoveFloor::Render()
{
	//PP::Model::UpdateTransform(model, transform);

	PP::Model::Render(model, PP::Shader::RenderShadow());
}

void StageMoveFloor::RenderSub()
{
	PP::Model::UpdateTransform(model, transform);

	PP::Model::Render(model, PP::Shader::DepthShadow());
}

void StageMoveFloor::DebugRender()
{

}

bool StageMoveFloor::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	PP::HitResult& hit)
{
	//return PP::Collision::RayCast(model, start, end, hit);

	// �O��̃��[���h�s��Ƌt�s������߂�
	DirectX::XMMATRIX OldTransform = DirectX::XMLoadFloat4x4(&oldTransform);
	DirectX::XMMATRIX InverseTransform = DirectX::XMMatrixInverse(nullptr, OldTransform);

	// �O��̃��[�J����Ԃł̃��C�ɕϊ�
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR LocalStart = DirectX::XMVector3TransformCoord(WorldStart, InverseTransform);
	DirectX::XMVECTOR LocalEnd = DirectX::XMVector3TransformCoord(WorldEnd, InverseTransform);

	// ���[�J����Ԃł̃��C�Ƃ̌�_�����߂�
	DirectX::XMFLOAT3 localStart, localEnd;
	DirectX::XMStoreFloat3(&localStart, LocalStart);
	DirectX::XMStoreFloat3(&localEnd, LocalEnd);

	PP::HitResult localHit;
	if (PP::Collision::RayCast(model, localStart, localEnd, localHit))
	{
		// �O��̃��[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
		// �O�񂩂獡��ɂ����ĕϊ����ꂽ���e������Ă���I�u�W�F�N�g�ɔ��f�����
		OldTransform = DirectX::XMLoadFloat4x4(&transform);

		DirectX::XMVECTOR LocalPosition = DirectX::XMLoadFloat3(&localHit.position);
		DirectX::XMVECTOR LocalNormal = DirectX::XMLoadFloat3(&localHit.normal);
		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(LocalPosition, OldTransform);
		DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(LocalNormal, OldTransform);

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
		DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);

		DirectX::XMStoreFloat3(&hit.position, WorldPosition);
		DirectX::XMStoreFloat3(&hit.normal, WorldNormal);
		DirectX::XMStoreFloat(&hit.distance, Dist);
		hit.materialIndex = localHit.materialIndex;

		// ��]�������Z�o
		hit.rotation.x = angle.x - oldAngle.x;
		hit.rotation.y = angle.y - oldAngle.y;
		hit.rotation.z = angle.z - oldAngle.z;

		return true;
	}
	return false;
}

void StageMoveFloor::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}