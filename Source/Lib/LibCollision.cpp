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
			// 2 -> 1への単位ベクトル算出
			DirectX::XMVECTOR Position1 = DirectX::XMLoadFloat3(&position1);
			DirectX::XMVECTOR Position2 = DirectX::XMLoadFloat3(&position2);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Position2, Position1);
			DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

			float length;
			DirectX::XMStoreFloat(&length, Length);

			// 距離判定
			float range = radius1 + radius2;

			// 交差してない場合処理を行わない
			if (length >= range) return false;

			// 1が2を押し出す
			Vec = DirectX::XMVector3Normalize(Vec);
			Vec = DirectX::XMVectorScale(Vec, range);

			// 補正値の算出
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
			// Aの足元がBの頭より上なら当たっていない
			if (position1.y > position2.y + height2)
				return false;

			// Aの頭がBの足元より下なら当たっていない
			if (position1.y + height1 < position2.y)
				return false;

			// XZ平面で範囲チェック
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

			// Aの足元がBの頭より上なら当たっていない
			if (spherePos.y - sphereRadius > cylinderPos.y + cylinderHeight)
				return false;

			// Aの頭がBの足元より下なら当たっていない
			if (spherePos.y + sphereRadius < cylinderPos.y)
				return false;

			// s→cの単位ベクトルを算出
			XMVECTOR sPos = XMLoadFloat3(&spherePos);
			XMVECTOR cPos = XMLoadFloat3(&cylinderPos);
			XMVECTOR Vec = XMVectorSubtract(cPos, sPos);
			XMVECTOR Length = XMVector3Length(Vec);
			float length;
			XMStoreFloat(&length, Length);

			// 距離判定
			float range = sphereRadius + cylinderRadius;
			if (length >= range) return false;

			// AがBを押し出す
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
			XMVECTOR WorldRayVec = XMVectorSubtract(WorldEnd, WorldStart); // スタート→エンド
			XMVECTOR WorldRayLength = XMVector3Length(WorldRayVec); // レイの長さ

			// ワールド空間のレイの長さ
			XMStoreFloat(&result.distance, WorldRayLength);

			bool hit = false;
			const ModelResource* resource = model->GetResource();
			for (const ModelResource::Mesh& mesh : resource->GetMeshes())
			{
				// メッシュノード取得
				const LibModel::Node& node = model->GetNodes().at(mesh.nodeIndex);

				// レイをワールド空間からローカル空間へ変更
				XMMATRIX WorldTransform = XMLoadFloat4x4(&node.worldTransform);
				XMMATRIX InverseWorldTransform = XMMatrixInverse(nullptr, WorldTransform);

				XMVECTOR Start = XMVector3TransformCoord(WorldStart, InverseWorldTransform);
				XMVECTOR End = XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
				XMVECTOR Vec = XMVectorSubtract(End, Start);
				XMVECTOR Dir = XMVector3Normalize(Vec);
				XMVECTOR Length = XMVector3Length(Vec);

				// レイの長さ
				float neart;
				XMStoreFloat(&neart, Length);

				// 三角形(面)との交差点 : 頂点バッファとインデックスバッファから三角形を抽出
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

						// ①三角形の頂点を抽出
						const ModelResource::Vertex& a = vertices.at(indices.at(index));
						const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
						const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

						XMVECTOR A = XMLoadFloat3(&a.position);
						XMVECTOR B = XMLoadFloat3(&b.position);
						XMVECTOR C = XMLoadFloat3(&c.position);

						// ②三角形の三辺ベクトルを算出
						XMVECTOR AB = XMVectorSubtract(B, A);
						XMVECTOR BC = XMVectorSubtract(C, B);
						XMVECTOR CA = XMVectorSubtract(A, C);

						// ③三角形の法線ベクトルを算出
						XMVECTOR Normal = XMVector3Cross(AB, BC);

						// ④内積の結果がプラスなら裏向き
						XMVECTOR Dot = XMVector3Dot(Dir, Normal);
						float dot;
						XMStoreFloat(&dot, Dot);
						if (dot >= 0.0f) continue;

						// ⑤レイと平面の交点を算出
						XMVECTOR V = XMVectorSubtract(A, Start);
						XMVECTOR T = XMVectorDivide(XMVector3Dot(Normal, V), Dot);
						float t;
						XMStoreFloat(&t, T);
						// 交点までの距離が今までに計算した最短距離より大きい場合はスキップ
						if (t < 0.0f || t > neart) continue;

						XMVECTOR Position = XMVectorAdd(XMVectorMultiply(Dir, T), Start); // 交点

						// ⑥交点が三角形の内側にあるか判定
						// １つめ
						XMVECTOR V1 = XMVectorSubtract(A, Position);
						XMVECTOR Cross1 = XMVector3Cross(V1, AB);
						XMVECTOR Dot1 = XMVector3Dot(Cross1, Normal);
						float dot1;
						XMStoreFloat(&dot1, Dot1);
						if (dot1 < 0.0f) continue;
						// ２つめ
						XMVECTOR V2 = XMVectorSubtract(B, Position);
						XMVECTOR Cross2 = XMVector3Cross(V2, BC);
						XMVECTOR Dot2 = XMVector3Dot(Cross2, Normal);
						float dot2;
						XMStoreFloat(&dot2, Dot2);
						if (dot2 < 0.0f) continue;
						// ３つめ
						XMVECTOR V3 = XMVectorSubtract(C, Position);
						XMVECTOR Cross3 = XMVector3Cross(V3, CA);
						XMVECTOR Dot3 = XMVector3Dot(Cross3, Normal);
						float dot3;
						XMStoreFloat(&dot3, Dot3);
						if (dot3 < 0.0f) continue;

						// 最短距離を更新
						neart = t;

						// 交点と法線を更新
						HitPosition = Position;
						HitNormal = (Normal);
						materialIndex = subset.materialIndex;

					}
				}

				if (materialIndex >= 0)
				{
					// ローカル空間からワールド空間へ変換
					XMVECTOR WorldPosition = XMVector3TransformCoord(HitPosition, WorldTransform);
					XMVECTOR WorldCrossVec = XMVectorSubtract(WorldPosition, WorldStart);
					XMVECTOR WorldCrossLength = XMVector3Length(WorldCrossVec);
					float distance;
					XMStoreFloat(&distance, WorldCrossLength);

					// ⑦ヒット情報の保存
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

				//	ワールド空間からノードのローカル空間に変換
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

		//	X軸専用
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

					//	法線をレイ方向に射影(内積)
					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	発射位置から3角形の一点を法線に射影
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					//	レイの長さ
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					//	交点算出
					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					//	2Dの外積を利用した内点判定
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

		//	Y軸専用
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

					//	法線をレイ方向に射影(内積)
					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	発射位置から3角形の一点を法線に射影
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					//	レイの長さ
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					//	交点算出
					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					//	2Dの外積を利用した内点判定
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

		//	Z軸専用
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

					//	法線をレイ方向に射影(内積)
					DirectX::XMVECTOR	Normal = DirectX::XMVector3Cross(AB, BC);
					DirectX::XMVECTOR	Dot = DirectX::XMVector3Dot(Normal, Dir);
					float	dot;
					DirectX::XMStoreFloat(&dot, Dot);
					if (dot >= 0)
						continue;

					//	発射位置から3角形の一点を法線に射影
					DirectX::XMVECTOR	V = DirectX::XMVectorSubtract(A, Start);
					//	レイの長さ
					DirectX::XMVECTOR	T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, V), Dot);
					float	t;
					DirectX::XMStoreFloat(&t, T);
					if (t < 0 || t > neart)
						continue;

					//	交点算出
					DirectX::XMVECTOR	Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

					//	2Dの外積を利用した内点判定
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

		//	通常版
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