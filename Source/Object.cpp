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

// �_���[�W��^����
bool Object::ApplyDamage(int damage, float invincibleTime)
{
	if (damage == 0) return false; // �_���[�W���Ȃ��ꍇ��false

	if (invincibleTimer > 0.0f) return false; // ���G���̂���false

	if (health <= 0) return false; // �̗͂��Ȃ��ꍇ��false

	health -= damage; // �_���[�W����
	invincibleTimer = invincibleTime;

	if (health <= 0) OnDead(); // ���S�ʒm
	else OnDamaged(); // �����Ă�̂Ł@�_���[�W�ʒm

	return true; // �_���[�W���������ꍇ��true
}

// �Ռ���^����
void Object::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// �ړ�
void Object::Move(float vx, float vz, float speed)
{
	moveVecX = vx;
	moveVecZ = vz;

	maxSpeed = speed;
}

// ����
void Object::Turn(float vx, float vz, float speed, float elapsedTime)
{
	speed *= elapsedTime;
	
	// �i�s�x�N�g�����[���̏ꍇ�͏������Ȃ�
	float length = sqrtf((vx * vx) + (vz * vz));
	if (length <= 0.0f) return;
	
	// �i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;
	
	// ���M�̉�]�l����O�������Z�o
	float frontX = sinf(wc.angle.y);
	float frontZ = cosf(wc.angle.y);
	
	// ��]�p�����߂邽�߂�2�̒P�ʃx�N�g���̓��ς��Z�o
	float dot = (vx * frontX) + (vz * frontZ);
	
	// ���ϒl�ł���-1.0�`1.0����2�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��̂ŁA���������]���x�𒲐�
	float rotation = 1.0f - dot;
	speed *= rotation;
	
	// ���E����̂��߂�2�̒P�ʃx�N�g��������ς��Z�o
	float cross = (vx * frontZ) - (vz * frontX);
	
	// 2D�̊O�ϒl�����������ō��E���肪�s��
	if (cross < 0.0f)
	{
		wc.angle.y -= speed; // ����]
	}
	else
	{
		wc.angle.y += speed; // �E��]
	}
}

// �W�����v
void Object::Jump(float speed)
{
	velocity.y = speed;
}

// ���͍X�V����
void Object::UpdateVelocity(float elapsedTime)
{
	float elapsedFrame = 60.0f * elapsedTime;

	UpdateVerticalVelocity(elapsedFrame);

	UpdateVerticalMove(elapsedTime);

	UpdateHorizontalVelocity(elapsedFrame);

	UpdateHorizontalMove(elapsedTime);
}

// ���G���ԍX�V����
void Object::UpdateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

// �������͍X�V����
void Object::UpdateVerticalVelocity(float elapsedFrame)
{
	// �d�͏���
	velocity.y += gravity * elapsedFrame;
}

// �����ړ��X�V����
void Object::UpdateVerticalMove(float elapsedTime)
{
	// ���������̈ړ���
	float my = velocity.y * elapsedTime;

	slopeRate = 0.0f;

	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	//DirectX::XMFLOAT3 normal = { 0, 1, 0 };

	// ������
	if (my < 0.0f)
	{
		// ���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { wc.position.x , wc.position.y + stepOffset, wc.position.z };
		// ���C�̏I���ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { wc.position.x, wc.position.y + my + gravity, wc.position.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		PP::HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �@���x�N�g���擾
			//normal = hit.normal;

			// �n�ʂɐڒn���Ă���
			wc.position = hit.position;

			// ��]
			wc.angle.y += hit.rotation.y;

			// �X�Η��̌v�Z
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// ���n����
			if (!isGround) OnLanding();
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
			// �󒆂ɕ����Ă���
			if (wc.position.y > -1.75f) // �ǔ������̗����΍�(��)
				wc.position.y += my;
			isGround = false;
		}

	}
	// �㏸��
	else if (my > 0.0f)
	{
		wc.position.y += my;
		isGround = false;
		//slopeRate = 0.0f;
	}

	// �n�ʂ̌����ɉ����悤��XZ����]
	/*{
		// Y�����@���x�N�g�������Ɍ����I�C���[�p��]���Z�o����
		wc.angle.x = atan2f(normal.z, normal.y);
		wc.angle.z = atan2f(normal.x, normal.y);
	
		float ax = atan2f(normal.z, normal.y);
		float az = atan2f(normal.x, normal.y);
	
		// ���`�⊮�Ŋ��炩�ɉ�]����
		wc.angle.x = PP::System::Lerp(wc.angle.x, ax, 0.2f);
		wc.angle.z = PP::System::Lerp(wc.angle.z, az, 0.2f);
	}*/
}

// �������͍X�V����
void Object::UpdateHorizontalVelocity(float elapsedFrame)
{
	// XZ���͂���������
	float length = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (length > 0.0f)
	{
		// ���C
		float friction = this->friction * elapsedFrame; 

		if (!IsGround()) friction *= airControl;

		// ���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			length -= friction;
			
			DirectX::XMFLOAT2 velo = { velocity.x, velocity.z };
			DirectX::XMVECTOR Velo = DirectX::XMLoadFloat2(&velo);
			Velo = DirectX::XMVector2Normalize(Velo);
			DirectX::XMStoreFloat2(&velo, Velo);
			velocity.x = velo.x * length;
			velocity.z = velo.y * length;

			// �P�ʃx�N�g����
			//float vx = velocity.x / length;
			//float vz = velocity.z / length;
			//velocity.x = vx * friction;
			//velocity.z = vz * friction;
		}
		// �������̑��͂����C�ȉ��ɂȂ����̂Ŗ�����
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	// XZ���ʂ̑��͂�����
	if (length <= maxSpeed)
	{
		// �ړ��x�N�g����0�łȂ��Ȃ����
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// ������
			float accel = this->accel * elapsedFrame;

			// �󒆂̏ꍇ�����͂����炷
			if (!IsGround()) accel *= airControl;

			// �ړ��x�N�g���ɂ���������
			velocity.x += moveVecX * accel;
			velocity.z += moveVecZ * accel;

			// �ő呬�x����
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
			// �����ŃK�^��Ȃ��悤�ɂ���
			if (!isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * elapsedFrame; // �߂�����ׂ����K�^���Ă�H
			}
		}
	}
	// �ړ��x�N�g�������Z�b�g
	moveVecX = 0.0f;
	moveVecZ = 0.0f;
}

// �����ړ��X�V����
void Object::UpdateHorizontalMove(float elapsedTime)
{
	// �������͗ʌv�Z
	float length = sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z));
	if (length > 0.0f)
	{
		// �����ړ��l
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { wc.position.x, wc.position.y + stepOffset, wc.position.z };
		DirectX::XMFLOAT3 end = { wc.position.x + mx, wc.position.y + stepOffset, wc.position.z + mz };

		// �ǔ���(���C�L���X�g)
		PP::HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// �ǂ̖@��
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			// ���˃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

			// �␳�ʒu�̌v�Z
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			// �ǂ�������փ��C�L���X�g
			PP::HitResult hit2;
			if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2))
			{
				// �ǂ�������ŕǂɓ�����Ȃ�������␳�ʒu�Ɉړ�
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
			// �ړ�
			wc.position.x += mx;
			wc.position.z += mz;
		}
	}
}