#include "Navigation.h"
#include <queue>
#include <stack>

bool NaviMesh::MeshDuplicate(NaviMesh* naviMesh)
{
	for (NaviNode* node : naviNodes)
	{
		if (naviMesh == node->nextNaviMesh)
		{
			// 既に登録されている
			return true;
		}
	}

	// 登録されていない
	return false;
}


Navigation* Navigation::instance = nullptr;

void Navigation::CreateStage(PP::ppModel& model)
{
	target = std::make_unique<NaviTarget>();

	// 読みこんだモデルデータのリソースを取得
	const PP::Class::ModelResource* resource = model->GetResource();

	// 取得したリソースのメッシュデータをまわす
	for (const PP::Class::ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// モデルデータのメッシュノードからワールド行列を取得
		const PP::Class::LibModel::Node& modelNode =
			model->GetNodes().at(mesh.nodeIndex);

		DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&modelNode.worldTransform);

		// ナビノード
		Node stageNode;

		// 三角形(面)との交差点 : 頂点バッファとインデックスバッファから三角形を抽出
		const std::vector<PP::Class::ModelResource::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;

		for (const PP::Class::ModelResource::Subset& subset : mesh.subsets)
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndex + i;

				// 三角形の頂点を抽出
				const PP::Class::ModelResource::Vertex& a = vertices.at(indices.at(index));
				const PP::Class::ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
				const PP::Class::ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR A = XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = XMLoadFloat3(&c.position);

				DirectX::XMVECTOR WorldPosition[3];
				WorldPosition[0] = DirectX::XMVector3TransformCoord(A, WorldTransform);
				WorldPosition[1] = DirectX::XMVector3TransformCoord(B, WorldTransform);
				WorldPosition[2] = DirectX::XMVector3TransformCoord(C, WorldTransform);

				DirectX::XMStoreFloat3(&stageNode.position[0], WorldPosition[0]);
				DirectX::XMStoreFloat3(&stageNode.position[1], WorldPosition[1]);
				DirectX::XMStoreFloat3(&stageNode.position[2], WorldPosition[2]);

				naviNodes.push_back(stageNode);
			}
		}
	}

	// ナビメッシュ作成
	for (int i = 0; i <= naviNodes.size(); i++)
	{
		if (i >= naviNodes.size()) break;

		NaviMesh* naviMesh = new NaviMesh();

		DirectX::XMFLOAT3 nodePosition[3] = 
		{
			naviNodes.at(i).position[0],
			naviNodes.at(i).position[1],
			naviNodes.at(i).position[2]
		};

		// 重心の算出
		{
			DirectX::XMFLOAT3 centerPosition;
			centerPosition.x = (nodePosition[0].x + nodePosition[1].x + nodePosition[2].x) / 3.0f;
			centerPosition.y = (nodePosition[0].y + nodePosition[1].y + nodePosition[2].y) / 3.0f;
			centerPosition.z = (nodePosition[0].z + nodePosition[1].z + nodePosition[2].z) / 3.0f;

			naviMesh->SetCenterPosition(centerPosition);
			naviMesh->GetPositions().push_back(nodePosition[0]);
			naviMesh->GetPositions().push_back(nodePosition[1]);
			naviMesh->GetPositions().push_back(nodePosition[2]);
		}

		naviMeshes.push_back(naviMesh);
	}

	// 隣のメッシュとの関連付け
	for (NaviMesh* naviMesh1 : naviMeshes)
	{
		for (NaviMesh* naviMesh2 : naviMeshes)
		{
			if (naviMesh1 == naviMesh2) continue;

			// 基準となるメッシュ
			for (int i = 0; i < naviMesh1->GetPositions().size(); i++)
			{
				// 関連となるメッシュ
				for (int j = 0; j < naviMesh2->GetPositions().size(); j++)
				{
					// 1点が連なっている
					if (naviMesh1->GetPositions().at(i).x == naviMesh2->GetPositions().at(j).x && 
						naviMesh1->GetPositions().at(i).y == naviMesh2->GetPositions().at(j).y &&
						naviMesh1->GetPositions().at(i).z == naviMesh2->GetPositions().at(j).z)
					{
						// 2点が連なっている(▲▼)
						for (int k = 0; k < naviMesh1->GetPositions().size(); k++)
						{
							if (naviMesh1->GetPositions().at(i).x == naviMesh1->GetPositions().at(k).x &&
								naviMesh1->GetPositions().at(i).y == naviMesh1->GetPositions().at(k).y &&
								naviMesh1->GetPositions().at(i).z == naviMesh1->GetPositions().at(k).z)
								continue;

							for (int l = 0; l < naviMesh2->GetPositions().size(); l++)
							{
								if (naviMesh1->GetPositions().at(k).x == naviMesh2->GetPositions().at(l).x && 
									naviMesh1->GetPositions().at(k).y == naviMesh2->GetPositions().at(l).y && 
									naviMesh1->GetPositions().at(k).z == naviMesh2->GetPositions().at(l).z)
								{
									// 既に登録されているなら
									if (naviMesh1->MeshDuplicate(naviMesh2)) break;

									NaviMesh::NaviNode* nextMesh = new NaviMesh::NaviNode();
									nextMesh->nextNaviMesh = naviMesh2;
									DirectX::XMVECTOR Center1 = DirectX::XMLoadFloat3(&naviMesh1->GetCenterPosition());
									DirectX::XMVECTOR Center2 = DirectX::XMLoadFloat3(&naviMesh2->GetCenterPosition());
									DirectX::XMVECTOR Length = DirectX::XMVector3Length(DirectX::XMVectorSubtract(Center1, Center2));
									DirectX::XMStoreFloat(&nextMesh->length, Length);
									naviMesh1->GetNaviNode().push_back(nextMesh);
								}
							}
						}
					}
				}
			}
		}
	}
}

void Navigation::DebugRender()
{
	for (int i = 0; i < rootMeshes.size(); i++)
	{
		NaviMesh* mesh3 = rootMeshes.at(i);
		DirectX::XMFLOAT3 position1 = { mesh3->GetPositions().at(0).x, mesh3->GetPositions().at(0).y, mesh3->GetPositions().at(0).z };
		DirectX::XMFLOAT3 position2 = { mesh3->GetPositions().at(1).x, mesh3->GetPositions().at(1).y, mesh3->GetPositions().at(1).z };
		DirectX::XMFLOAT3 position3 = { mesh3->GetPositions().at(2).x, mesh3->GetPositions().at(2).y, mesh3->GetPositions().at(2).z };

		PP::Debug::Primitive3D::DrawLine(position1, position2, { 0, 1, 0.5f, 1 });
		PP::Debug::Primitive3D::DrawLine(position2, position3, { 0, 1, 0.5f, 1 });
		PP::Debug::Primitive3D::DrawLine(position3, position1, { 0, 1, 0.5f, 1 });

	}
}

void Navigation::Clear()
{
	naviNodes.clear();
	for (NaviMesh* mesh : naviMeshes)
	{
		for (NaviMesh::NaviNode* node : mesh->GetNaviNode())
		{
			if (node != nullptr)
			{
				delete node;
				node = nullptr;
			}
		}
		if (mesh != nullptr)
		{
			delete mesh;
			mesh = nullptr;
		}
	}

	naviMeshes.clear();
}

void Navigation::CrossOperation(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2, DirectX::XMFLOAT3& cross)
{
	DirectX::XMVECTOR Pos1 = DirectX::XMLoadFloat3(&vec1);
	DirectX::XMVECTOR Pos2 = DirectX::XMLoadFloat3(&vec2);
	DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(Pos1, Pos2);
	DirectX::XMStoreFloat3(&cross, Cross);
}

void Navigation::SetTarget(const DirectX::XMFLOAT3& position, const float offset)
{
	DirectX::XMFLOAT3 start = { position.x, position.y + offset, position.z };
	DirectX::XMFLOAT3 end = { position.x, position.y - offset, position.z };

	// レイに当たったナビメッシュの取得
	target->function = GetOnNaviMesh(start, end, target->InMesh);
	target->position = position;

	//TK 現在のメッシュ位置を表示
	for (int i = 0; i < target->InMesh->GetPositions().size(); i++)
	{
		DirectX::XMFLOAT3 Position=target->InMesh->GetPositions().at(i);
		DirectX::XMFLOAT3 Position2;
		if (target->InMesh->GetPositions().size() - 1 == i)
		{
			Position2 = target->InMesh->GetPositions().at(0);
		}
		else
		{
			Position2 = target->InMesh->GetPositions().at(i + 1);
		}

		PP::Debug::Primitive3D::DrawLine(Position, Position2, {1,0,0,1});
	}
}

void Navigation::ClearNaviMeshSearch()
{
	for (NaviMesh* naviMesh : naviMeshes)
	{
		naviMesh->SetSearchFlag(false);
		naviMesh->ClearParent();
	}
}

void Navigation::GetNotContactPos(NaviMesh* now, NaviMesh* next, DirectX::XMFLOAT3& outPos)
{
	for (DirectX::XMFLOAT3 position1 : next->GetPositions())
	{
		bool flag = false;
		for (DirectX::XMFLOAT3 position2 : now->GetPositions())
		{
			// 面している
			if (position1.x == position2.x &&
				position1.y == position2.y &&
				position1.z == position2.z)
			{
				flag = true;
				break;
			}
		}
		// 面していない
		if (!flag)
		{
			outPos = position1;
			break;
		}
	}
}

void Navigation::GetContactPos(NaviMesh* now, NaviMesh* next, DirectX::XMFLOAT3* outPos)
{
	int ret = 0;
	for (DirectX::XMFLOAT3 position1 : next->GetPositions())
	{
		for (DirectX::XMFLOAT3 position2 : now->GetPositions())
		{
			// 面している
			if (position1.x == position2.x &&
				position1.y == position2.y &&
				position1.z == position2.z)
			{
				outPos[ret] = position1;
				ret++;
				break;
			}
		}
	}
}

void Navigation::SearchRoot(NaviMesh*& start, NaviMesh*& end)
{
	// ターゲットが範囲内にいない
	if (target->function == false) return;

	// 探索済みの経路を消去
	ClearNaviMeshSearch();

	// 探索候補のデータ
	std::queue<NaviMesh*> qMesh;
	qMesh.push(start);

	// 現在のメッシュ位置
	NaviMesh* mesh = nullptr;

	// ダイクストラで経路探査
	while (!qMesh.empty())
	{
		// メッシュデータの取り出し
		mesh = qMesh.front();

		// 削除
		qMesh.pop();

		// ノードを辿る
		for (NaviMesh::NaviNode* node : mesh->GetNaviNode())
		{
			if (node->nextNaviMesh->GetSearchFlag())
				continue;

			// 追加
			qMesh.push(node->nextNaviMesh);

			// 親の設定
			node->nextNaviMesh->SetParent(mesh);
			node->nextNaviMesh->SetSearchFlag(true);
		}

		// 目的地に到着
		if (mesh == end) break;
	}

	// 探索候補のデータ
	std::stack<NaviMesh*> sMesh;

	while (true)
	{
		sMesh.push(mesh);

		if (mesh == start) break;

		mesh = mesh->GetParent();
	}

	int size = sMesh.size();
	rootMeshes.clear();

	for (int i = 0; i < size; i++)
	{
		NaviMesh* mesh2 = sMesh.top();

		sMesh.pop();

		rootMeshes.push_back(mesh2);
	}
}

bool Navigation::FindRoot(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& outPos, const float offset)
{
	DirectX::XMFLOAT3 start = { position.x, position.y + offset, position.z };
	DirectX::XMFLOAT3 end = { position.x, position.y - offset, position.z };
	NaviMesh* mesh = nullptr;

	bool result = false;

	if (GetOnNaviMesh(start, end, mesh))
	{
		// 経路探査
		SearchRoot(mesh, GetNaviTarget()->InMesh);

		result = NearLengthRoot(position, GetNaviTarget()->position, outPos);

		return result;
	}

	return false;
}

bool Navigation::NearLengthRoot(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3& outPos)
{
	// ターゲットがNaviMeshか
	if (this->target->function == false) return false;

	// 同じか
	if (rootMeshes.size() <= 1)
	{
		outPos = target;
		return true;
	}

	DirectX::XMFLOAT3 contactPosition[2]; // 二つのメッシュが面している頂点
	DirectX::XMFLOAT3 contactNoPosition; // 二つのメッシュが面していない頂点

	// メッシュデータ算出
	NaviMesh* mesh = rootMeshes.at(0);
	NaviMesh* nextMesh = rootMeshes.at(1);

	// 二つのメッシュが面している頂点の算出
	GetContactPos(mesh, nextMesh, contactPosition);

	// 次のメッシュに面していない頂点の算出
	GetNotContactPos(mesh, nextMesh, contactNoPosition);

	// 二つのベクトルの垂直ベクトルを算出
	DirectX::XMFLOAT3 cross;
	DirectX::XMFLOAT3 contPos0;
	DirectX::XMFLOAT3 contPos1;
	contPos0 =
	{
		contactPosition[0].x - position.x,
		contactPosition[0].y - position.y,
		contactPosition[0].z - position.z
	};
	contPos1 =
	{
		contactPosition[1].x - position.x,
		contactPosition[1].y - position.y,
		contactPosition[1].z - position.z
	};
	CrossOperation(contPos1, contPos0, cross);

	if (cross.y > 0)
	{
		// crossを代用
		cross = contactPosition[0];
		contactPosition[0] = contactPosition[1];
		contactPosition[1] = cross;
		// 次のメッシュに面いていない頂点の算出
		GetNotContactPos(mesh, nextMesh, contactNoPosition);
	}

	// 角を算出
	for (int i = 0; i < rootMeshes.size() - 1; i++)
	{
		// 今と次のメッシュの算出
		mesh = rootMeshes.at(i);
		nextMesh = rootMeshes.at(i + 1);

		// 次のメッシュに面していない頂点の算出
		GetNotContactPos(mesh, nextMesh, contactNoPosition);

		// 位置を適用
		DirectX::XMFLOAT3 crossVec1;
		DirectX::XMFLOAT3 centerVec1;
		if (rootMeshes.size() - i > 2)
		{
			centerVec1 = // nextMesh->GetCenterPosition() - rootMeshes.at(i + 2)->GetCenterPosition();
			{ 
				nextMesh->GetCenterPosition().x - rootMeshes.at(i + 2)->GetCenterPosition().x,
				nextMesh->GetCenterPosition().y - rootMeshes.at(i + 2)->GetCenterPosition().y,
				nextMesh->GetCenterPosition().z - rootMeshes.at(i + 2)->GetCenterPosition().z 
			};
		}
		else
		{
			centerVec1 = // nextMesh->GetCenterPosition() - rootMeshes.at(i)->GetCenterPosition();
			{
				nextMesh->GetCenterPosition().x - rootMeshes.at(i)->GetCenterPosition().x,
				nextMesh->GetCenterPosition().y - rootMeshes.at(i)->GetCenterPosition().y,
				nextMesh->GetCenterPosition().z - rootMeshes.at(i)->GetCenterPosition().z
			};
		}

		DirectX::XMFLOAT3 vec = // contactNoPosition - mesh->GetCenterPosition();
		{
			contactNoPosition.x - mesh->GetCenterPosition().x,
			contactNoPosition.y - mesh->GetCenterPosition().y,
			contactNoPosition.z - mesh->GetCenterPosition().z
		};

		CrossOperation(centerVec1, vec, crossVec1);

		DirectX::XMFLOAT3 contNoPos = // contactNoPosition - position;
		{
			contactNoPosition.x - position.x,
			contactNoPosition.y - position.y,
			contactNoPosition.z - position.z
		};

		DirectX::XMFLOAT3 crossVec2;
		DirectX::XMFLOAT3 centerVec2;
		DirectX::XMVECTOR CenterVec2;
		constexpr float r = 0.5f;

		contPos0 =
		{
			contactPosition[0].x - position.x,
			contactPosition[0].y - position.y,
			contactPosition[0].z - position.z
		};
		contPos1 =
		{
			contactPosition[1].x - position.x,
			contactPosition[1].y - position.y,
			contactPosition[1].z - position.z
		};

		if (crossVec1.y <= 0)
		{
			CrossOperation(contPos0, contNoPos, crossVec2);
			if (crossVec2.y <= 0)
			{
				centerVec2 = // contactPosition[0] - mesh->GetCenterPosition();
				{
					contactPosition[0].x - mesh->GetCenterPosition().x,
					contactPosition[0].y - mesh->GetCenterPosition().y,
					contactPosition[0].z - mesh->GetCenterPosition().z
				};
				CenterVec2 = DirectX::XMLoadFloat3(&centerVec2);
				CenterVec2 = DirectX::XMVector3Normalize(CenterVec2);
				CenterVec2 = DirectX::XMVectorScale(CenterVec2, r);
				DirectX::XMStoreFloat3(&centerVec2, CenterVec2);

				target = // contactPosition[0] - centerVec2;
				{
					contactPosition[0].x - centerVec2.x,
					contactPosition[0].y - centerVec2.y,
					contactPosition[0].z - centerVec2.z
				};
				break;
			}

			CrossOperation(contPos1, contNoPos, crossVec2);

			if (crossVec2.y <= 0)
			{
				contactPosition[1] = contactNoPosition;
			}
		}
		else
		{
			CrossOperation(contPos1, contNoPos, crossVec2);

			if (crossVec2.y >= 0)
			{
				centerVec2 = // contactPosition[1] - mesh->GetCenterPosition();
				{
					contactPosition[1].x - mesh->GetCenterPosition().x,
					contactPosition[1].y - mesh->GetCenterPosition().y,
					contactPosition[1].z - mesh->GetCenterPosition().z
				};
				CenterVec2 = DirectX::XMLoadFloat3(&centerVec2);
				CenterVec2 = DirectX::XMVector3Normalize(CenterVec2);
				CenterVec2 = DirectX::XMVectorScale(CenterVec2, r);
				DirectX::XMStoreFloat3(&centerVec2, CenterVec2);

				target = // contactPosition[1] - centerVec2;
				{
					contactPosition[1].x - centerVec2.x,
					contactPosition[1].y - centerVec2.y,
					contactPosition[1].z - centerVec2.z
				};
				break;
			}

			CrossOperation(contPos0, contNoPos, crossVec2);

			if (crossVec2.y >= 0)
			{
				contactPosition[0] = contactNoPosition;
			}
		}
	}

	if (target.x + target.y + target.z == 0) return false;

	outPos = target;

	return true;
}

bool Navigation::GetOnNaviMesh(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, NaviMesh*& mesh)
{
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);

	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
	DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

	float length;
	DirectX::XMStoreFloat(&length, Length);

	// 最短距離
	float nearRange = FLT_MAX;
	// 最短距離のメッシュ
	NaviMesh* nearMesh = nullptr;

	for (NaviMesh* naviMesh : naviMeshes)
	{
		DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&naviMesh->GetPositions().at(0));
		DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&naviMesh->GetPositions().at(1));
		DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&naviMesh->GetPositions().at(2));

		for (int i = 0; i < 3; i++)
		{
			DirectX::XMFLOAT3 vec = // GetPositions().at(i) - start;
			{
				naviMesh->GetPositions().at(i).x - start.x,
				naviMesh->GetPositions().at(i).y - start.y,
				naviMesh->GetPositions().at(i).z - start.z
			};
			DirectX::XMVECTOR Vec2 = DirectX::XMLoadFloat3(&vec);
			Vec2 = DirectX::XMVector3Length(Vec2);
			float range;
			DirectX::XMStoreFloat(&range, Vec2);
			if (nearRange >= range)
			{
				nearRange = range;
				nearMesh = naviMesh;
			}
		}

		//三角形の三辺ベクトルを算出
		DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
		DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
		DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
		
		//三角形の法線ベクトルの算出
		
		DirectX::XMVECTOR Normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, BC));
		
		//内積の値がプラスなら裏向き
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
		float dot;
		DirectX::XMStoreFloat(&dot, Dot);
		if (dot >= 0)continue;//裏向き

		// レイと平面の交差点
		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(A, Start);
		DirectX::XMVECTOR T = DirectX::XMVectorDivide(DirectX::XMVector3Dot(V, Normal), Dot);
		float t;
		DirectX::XMStoreFloat(&t, T);
		// 交点までの距離が今までに計算した最短距離より大きい場合はスキップ
		if (t < 0.0f || t > length) continue;

		DirectX::XMVECTOR Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, T), Start);

		// 交点が三角形の内側にあるか判定
		// １
		{
			DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(Position, A);
			DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(AB, V1);
			DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal);
			float dot1;
			DirectX::XMStoreFloat(&dot1, Dot1);
			if (dot1 < 0.0f) continue;
		}
		// ２
		{
			DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(Position, B);
			DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(BC, V2);
			DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal);
			float dot2;
			DirectX::XMStoreFloat(&dot2, Dot2);
			if (dot2 < 0.0f) continue;
		}
		// ３
		{
			DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(Position, C);
			DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(CA, V3);
			DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal);
			float dot3;
			DirectX::XMStoreFloat(&dot3, Dot3);
			if (dot3 < 0.0f) continue;
		}

		mesh = naviMesh;

		return true;
	}
	mesh = nearMesh;

	return true;
}