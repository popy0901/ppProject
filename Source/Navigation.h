#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "Lib/Lib.h"

class NaviMesh
{
public:

	struct NaviNode
	{
		NaviMesh* nextNaviMesh;

		float length;
	};

	// 既にNaviNodesに登録されているかどうか
	bool MeshDuplicate(NaviMesh* mesh);

	void SetCenterPosition(const DirectX::XMFLOAT3& centerPosition) 
	{ this->centerPosition = centerPosition; }

	const DirectX::XMFLOAT3& GetCenterPosition() const { return centerPosition; }

	void SetSearchFlag(const bool flag) { searchFlag = flag; }

	const bool GetSearchFlag() const { return searchFlag; }

	void SetParent(NaviMesh* parent) { this->parent = parent; }

	NaviMesh*& GetParent() { return parent; }

	void ClearParent() { parent = nullptr; }

	std::vector<DirectX::XMFLOAT3>& GetPositions() { return positions; }

	std::vector<NaviNode*>& GetNaviNode() { return naviNodes; }

private:

	std::vector<DirectX::XMFLOAT3> positions;

	// 子供ノード
	std::vector<NaviNode*> naviNodes;

	// 親のメッシュ(探索時に用いる)
	NaviMesh* parent;

	DirectX::XMFLOAT3 centerPosition;

	// 探索フラグ
	bool searchFlag = false;

};

class Navigation
{
public:

	struct Node
	{
		DirectX::XMFLOAT3 position[3];
	};

	struct NaviTarget
	{
		NaviMesh* InMesh;

		DirectX::XMFLOAT3 position;

		bool function;
	};


	Navigation() { instance = this; }
	~Navigation() {}

	// ナビ用のステージ作成
	void CreateStage(PP::ppModel& model);

	// ナビデバック描画
	void DebugRender();

	// 消去
	void Clear();

	// クロス算
	void CrossOperation(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2, DirectX::XMFLOAT3& cross);

	// ターゲットを設定
	void SetTarget(const DirectX::XMFLOAT3& position, const float offset = 1.0f);

	// 経路探査の探索済みを消す
	void ClearNaviMeshSearch();

	// 次のメッシュに面していない頂点の算出
	void GetNotContactPos(NaviMesh* now, NaviMesh* next, DirectX::XMFLOAT3& outPos);

	// 次のメッシュに面している頂点の算出
	void GetContactPos(NaviMesh* now, NaviMesh* next, DirectX::XMFLOAT3* outPos);

	// 経路探査
	void SearchRoot(NaviMesh*& start, NaviMesh*& end);

	// 経路検索
	bool FindRoot(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& outPos, const float offset = 1.0f);

	// NaviMeshから実際の距離(経路)を算出
	bool NearLengthRoot(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3& outPos);

	// レイに当たったナビメッシュの取得
	bool GetOnNaviMesh(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, NaviMesh*& mesh);

	NaviTarget* GetNaviTarget() { return target.get(); }

	static Navigation& Instance() { return *instance; }

private:

	// 地面メッシュあたり判定
	std::vector<Node> naviNodes;

	// ナビメッシュ
	std::vector<NaviMesh*> naviMeshes;

	// 始点から終点までのエッジ
	std::vector<NaviMesh*> rootMeshes;

	// ターゲットの情報
	std::unique_ptr<NaviTarget> target;

	static Navigation* instance;

};