#include "LibCollision.h"



namespace PP
{
	namespace Class
	{
		bool LibCollision::SphereVsSphere(
			const DirectX::XMFLOAT3& position1,
			float radius1,
			const DirectX::XMFLOAT3& position2,
			float radius2,
			DirectX::XMFLOAT3& outPosition2)
		{
			// 2 -> 1�ւ̒P�ʃx�N�g���Z�o
			DirectX::XMVECTOR Position1 = DirectX::XMLoadFloat3(&position1);
			DirectX::XMVECTOR Position2 = DirectX::XMLoadFloat3(&position2);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position2, Position1);
			DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

			float length;
			DirectX::XMStoreFloat(&length, Length);

			// ��������
			float range = radius1 + radius2;

			// �������ĂȂ��ꍇ�������s��Ȃ�
			if (length >= range) return false;

			// 1��2�������o��
			Vec = DirectX::XMVector3Normalize(Vec);
			Vec = DirectX::XMVectorScale(Vec, range);

			// �␳�l�̎Z�o
			DirectX::XMVECTOR OutPosition2 = DirectX::XMVectorAdd(Position1, Vec);
			DirectX::XMStoreFloat3(&outPosition2, OutPosition2);

			return true;
		}

		bool LibCollision::CylinderVsCylinder(
			const DirectX::XMFLOAT3& position1,
			float radius1,
			float height1,
			const DirectX::XMFLOAT3& position2,
			float radius2,
			float height2,
			DirectX::XMFLOAT3& outPosition2)
		{
			// A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
			if (position1.y > position2.y + height2)
				return false;

			// A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
			if (position1.y + height1 < position2.y)
				return false;

			// XZ���ʂŔ͈̓`�F�b�N
			float vx = position2.x - position1.x;
			float vz = position2.z - position1.z;
			float vec = sqrtf((vx * vx) + (vz * vz));
			float range = radius1 + radius2;

			if (vec >= range) return false;

			vx /= vec;
			vz /= vec;
			outPosition2.x = position1.x + vx * range;
			outPosition2.y = position2.y;
			outPosition2.z = position1.z + vz * range;

			return true;
		}

		bool LibCollision::IntersectSphereVsCylinder(
			const DirectX::XMFLOAT3& spherePos,
			float sphereRadius,
			const DirectX::XMFLOAT3& cylinderPos,
			float cylinderRadius,
			float cylinderHeight,
			DirectX::XMFLOAT3& outCylinderPos)
		{
			using namespace DirectX;

			// A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
			if (spherePos.y - sphereRadius > cylinderPos.y + cylinderHeight)
				return false;

			// A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
			if (spherePos.y + sphereRadius < cylinderPos.y)
				return false;

			// s��c�̒P�ʃx�N�g�����Z�o
			XMVECTOR sPos = XMLoadFloat3(&spherePos);
			XMVECTOR cPos = XMLoadFloat3(&cylinderPos);
			XMVECTOR Vec = XMVectorSubtract(cPos, sPos);
			XMVECTOR Length = XMVector3Length(Vec);
			float length;
			XMStoreFloat(&length, Length);

			// ��������
			float range = sphereRadius + cylinderRadius;
			if (length >= range) return false;

			// A��B�������o��
			Vec = XMVector3Normalize(Vec);
			Vec = XMVectorScale(Vec, (range - length));
			cPos = XMVectorAdd(cPos, Vec);
			XMStoreFloat3(&outCylinderPos, cPos);

			return true;

		}

		bool LibCollision::IntersectRayVsMode(
			const DirectX::XMFLOAT3& start,
			const DirectX::XMFLOAT3& end,
			const LibModel* model,
			HitResult& result)
		{
			using namespace DirectX;

			XMVECTOR WorldStart = XMLoadFloat3(&start);
			XMVECTOR WorldEnd = XMLoadFloat3(&end);
			XMVECTOR WorldRayVec = XMVectorSubtract(WorldEnd, WorldStart); // �X�^�[�g���G���h
			XMVECTOR WorldRayLength = XMVector3Length(WorldRayVec); // ���C�̒���

			// ���[���h��Ԃ̃��C�̒���
			XMStoreFloat(&result.distance, WorldRayLength);

			bool hit = false;
			const ModelResource* resource = model->GetResource();
			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				// ���b�V���m�[�h�擾
				const LibModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

				// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ύX
				XMMATRIX WorldTransform = XMLoadFloat4x4(&node.worldTransform);
				XMMATRIX InverseWorldTransform = XMMatrixInverse(nullptr, WorldTransform);

				XMVECTOR Start = XMVector3TransformCoord(WorldStart, InverseWorldTransform);
				XMVECTOR End = XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
				XMVECTOR Vec = XMVectorSubtract(End, Start);
				XMVECTOR Dir = XMVector3Normalize(Vec);
				XMVECTOR Length = XMVector3Length(Vec);

				// ���C�̒���
				float neart;
				XMStoreFloat(&neart, Length);

				// �O�p�`(��)�Ƃ̌����_ : ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@����O�p�`�𒊏o
				const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
				const std::vector<UINT> indices = mesh.indices;

				int materialIndex = -1;
				XMVECTOR HitPosition;
				XMVECTOR HitNormal;

				for (const ModelResource::Subset& subset : mesh.subsets)
				{
					for (UINT i = 0; i < subset.indexCount; i += 3)
					{
						UINT index = subset.startIndex + i;

						// �@�O�p�`�̒��_�𒊏o
						const ModelResource::Vertex& a = vertices.at(indices.at(index));
						const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
						const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

						XMVECTOR A = XMLoadFloat3(&a.position);
						XMVECTOR B = XMLoadFloat3(&b.position);
						XMVECTOR C = XMLoadFloat3(&c.position);

						// �A�O�p�`�̎O�Ӄx�N�g�����Z�o
						XMVECTOR AB = XMVectorSubtract(B, A);
						XMVECTOR BC = XMVectorSubtract(C, B);
						XMVECTOR CA = XMVectorSubtract(A, C);

						// �B�O�p�`�̖@���x�N�g�����Z�o
						XMVECTOR Normal = XMVector3Cross(AB, BC);

						// �C���ς̌��ʂ��v���X�Ȃ痠����
						XMVECTOR Dot = XMVector3Dot(Dir, Normal);
						float dot;
						XMStoreFloat(&dot, Dot);
						if (dot >= 0.0f) continue;

						// �D���C�ƕ��ʂ̌�_���Z�o
						XMVECTOR V = XMVectorSubtract(A, Start);
						XMVECTOR T = XMVectorDivide(XMVector3Dot(Normal, V), Dot);
						float t;
						XMStoreFloat(&t, T);
						// ��_�܂ł̋��������܂łɌv�Z�����ŒZ�������傫���ꍇ�̓X�L�b�v
						if (t < 0.0f || t > neart) continue;

						XMVECTOR Position = XMVectorAdd(XMVectorMultiply(Dir, T), Start); // ��_

						// �E��_���O�p�`�̓����ɂ��邩����
						// �P��
						XMVECTOR V1 = XMVectorSubtract(A, Position);
						XMVECTOR Cross1 = XMVector3Cross(V1, AB);
						XMVECTOR Dot1 = XMVector3Dot(Cross1, Normal);
						float dot1;
						XMStoreFloat(&dot1, Dot1);
						if (dot1 < 0.0f) continue;
						// �Q��
						XMVECTOR V2 = XMVectorSubtract(B, Position);
						XMVECTOR Cross2 = XMVector3Cross(V2, BC);
						XMVECTOR Dot2 = XMVector3Dot(Cross2, Normal);
						float dot2;
						XMStoreFloat(&dot2, Dot2);
						if (dot2 < 0.0f) continue;
						// �R��
						XMVECTOR V3 = XMVectorSubtract(C, Position);
						XMVECTOR Cross3 = XMVector3Cross(V3, CA);
						XMVECTOR Dot3 = XMVector3Dot(Cross3, Normal);
						float dot3;
						XMStoreFloat(&dot3, Dot3);
						if (dot3 < 0.0f) continue;

						// �ŒZ�������X�V
						neart = t;

						// ��_�Ɩ@�����X�V
						HitPosition = Position;
						HitNormal = (Normal);
						materialIndex = subset.materialIndex;

					}
				}

				if (materialIndex >= 0)
				{
					// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
					XMVECTOR WorldPosition = XMVector3TransformCoord(HitPosition, WorldTransform);
					XMVECTOR WorldCrossVec = XMVectorSubtract(WorldPosition, WorldStart);
					XMVECTOR WorldCrossLength = XMVector3Length(WorldCrossVec);
					float distance;
					XMStoreFloat(&distance, WorldCrossLength);

					// �F�q�b�g���̕ۑ�
					if (result.distance > distance)
					{
						XMVECTOR WorldNormal = XMVector3TransformNormal(HitNormal, WorldTransform);

						result.distance = distance;
						result.materialIndex = materialIndex;
						XMStoreFloat3(&result.position, WorldPosition);
						XMStoreFloat3(&result.normal, XMVector3Normalize(WorldNormal));
						hit = true;
					}
				}
			}
			return hit;
		}

		bool	LibCollision::IntersectRayVsModel(
			const DirectX::XMFLOAT3& start,
			const DirectX::XMFLOAT3& end,
			const LibModel* model,
			HitResult& result)
		{
			DirectX::XMVECTOR	WorldStart = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR	WorldEnd = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR	WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
			DirectX::XMVECTOR	WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

			DirectX::XMStoreFloat(&result.distance, WorldRayLength);

			bool	hit = false;
			const	ModelResource* resource = model->GetResource();
			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				const	LibModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

				//	���[���h��Ԃ���m�[�h�̃��[�J����Ԃɕϊ�
				DirectX::XMMATRIX	WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
				DirectX::XMMATRIX	InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

				DirectX::XMVECTOR	Start = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
				DirectX::XMVECTOR	End = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
				DirectX::XMVECTOR	Vec = DirectX::XMVectorSubtract(End, Start);
				DirectX::XMVECTOR	Dir = DirectX::XMVector3Normalize(Vec);
				DirectX::XMVECTOR	Length = DirectX::XMVector3Length(Vec);

				float	neart;
				DirectX::XMStoreFloat(&neart, Length);

				int	materialIndex = -1;
				DirectX::XMVECTOR	HitPosition, HitNormal;

				DirectX::XMFLOAT3	vec;
				DirectX::XMStoreFloat3(&vec, Vec);
				if (vec.x == 0 && vec.z == 0)
				{
					materialIndex = IntersectRayVsModelY(Start, Dir, neart, mesh, HitPosition, HitNormal);
				}
				else	if (vec.x == 0 && vec.y == 0)
				{
					materialIndex = IntersectRayVsModelZ(Start, Dir, neart, mesh, HitPosition, HitNormal);
				}
				else	if (vec.y == 0 && vec.z == 0)
				{
					materialIndex = IntersectRayVsModelX(Start, Dir, neart, mesh, HitPosition, HitNormal);
				}
				else
				{
					materialIndex = IntersectRayVsModel(Start, Dir, neart, mesh, HitPosition, HitNormal);
				}
				if (materialIndex >= 0)
				{
					DirectX::XMVECTOR	WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
					DirectX::XMVECTOR	WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
					DirectX::XMVECTOR	WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
					float	distance;
					DirectX::XMStoreFloat(&distance, WorldCrossLength);

					if (result.distance > distance)
					{
						DirectX::XMVECTOR	WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
						result.distance = distance;
						result.materialIndex = materialIndex;
						DirectX::XMStoreFloat3(&result.position, WorldPosition);
						DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
						hit = true;
					}
				}
			}
			return	hit;
		}

		//	X����p
		int	LibCollision::IntersectRayVsModelX(const DirectX::XMVECTOR& Start,
			const DirectX::XMVECTOR& Dir,
			float	neart,
			const ModelResource::Mesh& mesh,
			DirectX::XMVECTOR& hitPosition,
			DirectX::XMVECTOR& hitNormal)
		{
			const	std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
			const	std::vector<UINT>& indices = mesh.indices;
			const	ModelResource::Vertex* pVertices = vertices.data();
			const	UINT* pIndices = indices.data();

			int	materialIndex = -1;
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				for (UINT ii = 0; ii < subset.indexCount; ii += 3)
				{
					UINT	index = subset.startIndex + ii;
					const	ModelResource::Vertex& a = pVertices[pIndices[index + 0]];
					const	ModelResource::Vertex& b = pVertices[pIndices[index + 1]];
					const	ModelResource::Vertex& c = pVertices[pIndices[index + 2]];

					DirectX::XMVECTOR	A = DirectX::XMLoadFloat3(&a.position);
					DirectX::XMVECTOR	B = DirectX::XMLoadFloat3(&b.position);
					DirectX::XMVECTOR	C = DirectX::XMLoadFloat3(&c.position);

					DirectX::XMVECTOR	AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR	BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR	CA = DirectX::XMVectorSubtract(A, C);

					//	�@�������C�����Ɏˉe(����)
					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	���ˈʒu����3�p�`�̈�_��@���Ɏˉe
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					//	���C�̒���
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					//	��_�Z�o
					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					//	2D�̊O�ς𗘗p�������_����
					DirectX::XMFLOAT3	position, ab, bc, ca;
					DirectX::XMStoreFloat3(&position, Position);
					DirectX::XMStoreFloat3(&ab, AB);
					if ((a.position.y - position.y) * ab.z - (a.position.z - position.z) * ab.y > 0)
						continue;

					DirectX::XMStoreFloat3(&bc, BC);
					if ((b.position.y - position.y) * bc.z - (b.position.z - position.z) * bc.y > 0)
						continue;

					DirectX::XMStoreFloat3(&ca, CA);
					if ((c.position.y - position.y) * ca.z - (c.position.z - position.z) * ca.y > 0)
						continue;
					neart = t;
					materialIndex = subset.materialIndex;
					hitPosition = Position;
					hitNormal = Normal;
				}
			}
			return	materialIndex;
		}

		//	Y����p
		int	LibCollision::IntersectRayVsModelY(const DirectX::XMVECTOR& Start,
			const DirectX::XMVECTOR& Dir,
			float	neart,
			const ModelResource::Mesh& mesh,
			DirectX::XMVECTOR& hitPosition,
			DirectX::XMVECTOR& hitNormal)
		{
			const	std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
			const	std::vector<UINT>& indices = mesh.indices;
			const	ModelResource::Vertex* pVertices = vertices.data();
			const	UINT* pIndices = indices.data();

			int	materialIndex = -1;
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				for (UINT ii = 0; ii < subset.indexCount; ii += 3)
				{
					UINT	index = subset.startIndex + ii;
					const	ModelResource::Vertex& a = pVertices[pIndices[index + 0]];
					const	ModelResource::Vertex& b = pVertices[pIndices[index + 1]];
					const	ModelResource::Vertex& c = pVertices[pIndices[index + 2]];

					DirectX::XMVECTOR	A = DirectX::XMLoadFloat3(&a.position);
					DirectX::XMVECTOR	B = DirectX::XMLoadFloat3(&b.position);
					DirectX::XMVECTOR	C = DirectX::XMLoadFloat3(&c.position);

					DirectX::XMVECTOR	AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR	BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR	CA = DirectX::XMVectorSubtract(A, C);

					//	�@�������C�����Ɏˉe(����)
					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	���ˈʒu����3�p�`�̈�_��@���Ɏˉe
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					//	���C�̒���
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					//	��_�Z�o
					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					//	2D�̊O�ς𗘗p�������_����
					DirectX::XMFLOAT3	position, ab, bc, ca;
					DirectX::XMStoreFloat3(&position, Position);
					DirectX::XMStoreFloat3(&ab, AB);
					if ((a.position.x - position.x) * ab.z - (a.position.z - position.z) * ab.x > 0)
						continue;

					DirectX::XMStoreFloat3(&bc, BC);
					if ((b.position.x - position.x) * bc.z - (b.position.z - position.z) * bc.x > 0)
						continue;

					DirectX::XMStoreFloat3(&ca, CA);
					if ((c.position.x - position.x) * ca.z - (c.position.z - position.z) * ca.x > 0)
						continue;

					neart = t;
					materialIndex = subset.materialIndex;
					hitPosition = Position;
					hitNormal = Normal;
				}
			}
			return	materialIndex;
		}

		//	Z����p
		int	LibCollision::IntersectRayVsModelZ(const DirectX::XMVECTOR& Start,
			const DirectX::XMVECTOR& Dir,
			float	neart,
			const ModelResource::Mesh& mesh,
			DirectX::XMVECTOR& hitPosition,
			DirectX::XMVECTOR& hitNormal)
		{
			const	std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
			const	std::vector<UINT>& indices = mesh.indices;
			const	ModelResource::Vertex* pVertices = vertices.data();
			const	UINT* pIndices = indices.data();

			int	materialIndex = -1;
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				for (UINT ii = 0; ii < subset.indexCount; ii += 3)
				{
					UINT	index = subset.startIndex + ii;
					const	ModelResource::Vertex& a = pVertices[pIndices[index + 0]];
					const	ModelResource::Vertex& b = pVertices[pIndices[index + 1]];
					const	ModelResource::Vertex& c = pVertices[pIndices[index + 2]];

					DirectX::XMVECTOR	A = DirectX::XMLoadFloat3(&a.position);
					DirectX::XMVECTOR	B = DirectX::XMLoadFloat3(&b.position);
					DirectX::XMVECTOR	C = DirectX::XMLoadFloat3(&c.position);

					DirectX::XMVECTOR	AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR	BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR	CA = DirectX::XMVectorSubtract(A, C);

					//	�@�������C�����Ɏˉe(����)
					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	���ˈʒu����3�p�`�̈�_��@���Ɏˉe
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					//	���C�̒���
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					//	��_�Z�o
					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					//	2D�̊O�ς𗘗p�������_����
					DirectX::XMFLOAT3	position, ab, bc, ca;
					DirectX::XMStoreFloat3(&position, Position);
					DirectX::XMStoreFloat3(&ab, AB);
					if ((a.position.x - position.x) * ab.y - (a.position.y - position.y) * ab.x > 0)
						continue;

					DirectX::XMStoreFloat3(&bc, BC);
					if ((b.position.x - position.x) * bc.y - (b.position.y - position.y) * bc.x > 0)
						continue;

					DirectX::XMStoreFloat3(&ca, CA);
					if ((c.position.x - position.x) * ca.y - (c.position.y - position.y) * ca.x > 0)
						continue;

					neart = t;
					materialIndex = subset.materialIndex;
					hitPosition = Position;
					hitNormal = Normal;
				}
			}
			return	materialIndex;
		}

		//	�ʏ��
		int	LibCollision::IntersectRayVsModel(const DirectX::XMVECTOR& Start,
			const DirectX::XMVECTOR& Dir,
			float	neart,
			const ModelResource::Mesh& mesh,
			DirectX::XMVECTOR& hitPosition,
			DirectX::XMVECTOR& hitNormal)
		{
			const	std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
			const	std::vector<UINT>& indices = mesh.indices;
			const	ModelResource::Vertex* pVertices = vertices.data();
			const	UINT* pIndices = indices.data();

			int	materialIndex = -1;
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				for (UINT ii = 0; ii < subset.indexCount; ii += 3)
				{
					UINT	index = subset.startIndex + ii;
					const	ModelResource::Vertex& a = pVertices[pIndices[index + 0]];
					const	ModelResource::Vertex& b = pVertices[pIndices[index + 1]];
					const	ModelResource::Vertex& c = pVertices[pIndices[index + 2]];

					DirectX::XMVECTOR	A = DirectX::XMLoadFloat3(&a.position);
					DirectX::XMVECTOR	B = DirectX::XMLoadFloat3(&b.position);
					DirectX::XMVECTOR	C = DirectX::XMLoadFloat3(&c.position);

					DirectX::XMVECTOR	AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR	BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR	CA = DirectX::XMVectorSubtract(A, C);

					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);

					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T)
						, Start);

					DirectX::XMVECTOR	V1 = DirectX::XMVectorSubtract(A, Position);
					DirectX::XMVECTOR	Cross1 = DirectX::XMVector3Cross(V1, AB);
					DirectX::XMVECTOR	Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
					float	dot1;
					DirectX::XMStoreFloat(&dot1, Dot1);
					if (dot1 < 0)
						continue;

					DirectX::XMVECTOR	V2 = DirectX::XMVectorSubtract(B, Position);
					DirectX::XMVECTOR	Cross2 = DirectX::XMVector3Cross(V2, BC);
					DirectX::XMVECTOR	Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
					float	dot2;
					DirectX::XMStoreFloat(&dot2, Dot2);
					if (dot2 < 0)
						continue;

					DirectX::XMVECTOR	V3 = DirectX::XMVectorSubtract(C, Position);
					DirectX::XMVECTOR	Cross3 = DirectX::XMVector3Cross(V3, CA);
					DirectX::XMVECTOR	Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
					float	dot3;
					DirectX::XMStoreFloat(&dot3, Dot3);
					if (dot3 < 0)
						continue;

					neart = t;
					materialIndex = subset.materialIndex;
					hitPosition = Position;
					hitNormal = Normal;
				}
			}
			return	materialIndex;
		}


	}
}