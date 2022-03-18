#pragma once

#include "Graphics/Model.h"

namespace PP
{
	struct HitResult
	{
		DirectX::XMFLOAT3 position = { 0, 0, 0 }; // ���C�ƃ|���S���̌����_
		DirectX::XMFLOAT3 normal = { 0, 0, 0 }; // �Փ˂����|���S���̖@���x�N�g��
		DirectX::XMFLOAT3 rotation = { 0, 0, 0 }; // ��]��
		float distance = 0.0f; // ���C�̎n�_�����_�܂ł̋���
		int materialIndex = -1; // �Փ˂����|���S���̃}�e���A���ԍ�
	};
	namespace Class
	{
		class LibCollision
		{
		public:

			LibCollision() {}
			~LibCollision() {}

			bool SphereVsSphere(
				const DirectX::XMFLOAT3& position1,
				float radius1,
				const DirectX::XMFLOAT3& position2,
				float radius2,
				DirectX::XMFLOAT3& outPosition2/*position2�̕␳��̈ʒu*/);

			bool CylinderVsCylinder(
				const DirectX::XMFLOAT3& position1,
				float radius1,
				float height1,
				const DirectX::XMFLOAT3& position2,
				float radius2,
				float height2,
				DirectX::XMFLOAT3& outPosition2);

			bool IntersectSphereVsCylinder(
				const DirectX::XMFLOAT3& spherePos,
				float sphereRadius,
				const DirectX::XMFLOAT3& cylinderPos,
				float cylinderRadius,
				float cylinderHeight,
				DirectX::XMFLOAT3& outCylinderPos);

			bool IntersectRayVsMode(
				const DirectX::XMFLOAT3& start,
				const DirectX::XMFLOAT3& end,
				const LibModel* model,
				HitResult& result);

			// ���C�L���X�g //
			bool IntersectRayVsModel(
				const DirectX::XMFLOAT3& start,
				const DirectX::XMFLOAT3& end,
				const LibModel* model,
				HitResult& result);

			// X
			int IntersectRayVsModelX(const DirectX::XMVECTOR& Start,
				const DirectX::XMVECTOR& Dir,
				float	neart,
				const ModelResource::Mesh& mesh,
				DirectX::XMVECTOR& hitPosition,
				DirectX::XMVECTOR& hitNormal);

			// Y
			int IntersectRayVsModelY(const DirectX::XMVECTOR& Start,
				const DirectX::XMVECTOR& Dir,
				float	neart,
				const ModelResource::Mesh& mesh,
				DirectX::XMVECTOR& hitPosition,
				DirectX::XMVECTOR& hitNormal);

			// Z
			int IntersectRayVsModelZ(const DirectX::XMVECTOR& Start,
				const DirectX::XMVECTOR& Dir,
				float	neart,
				const ModelResource::Mesh& mesh,
				DirectX::XMVECTOR& hitPosition,
				DirectX::XMVECTOR& hitNormal);

			// �ʏ�
			int IntersectRayVsModel(const DirectX::XMVECTOR& Start,
				const DirectX::XMVECTOR& Dir,
				float	neart,
				const ModelResource::Mesh& mesh,
				DirectX::XMVECTOR& hitPosition,
				DirectX::XMVECTOR& hitNormal);

		};
		
	}
}