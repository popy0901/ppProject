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

	// ����NaviNodes�ɓo�^����Ă��邩�ǂ���
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

	// �q���m�[�h
	std::vector<NaviNode*> naviNodes;

	// �e�̃��b�V��(�T�����ɗp����)
	NaviMesh* parent;

	DirectX::XMFLOAT3 centerPosition;

	// �T���t���O
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

	// �i�r�p�̃X�e�[�W�쐬
	void CreateStage(PP::ppModel& model);

	// �i�r�f�o�b�N�`��
	void DebugRender();

	// ����
	void Clear();

	// �N���X�Z
	void CrossOperation(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2, DirectX::XMFLOAT3& cross);

	// �^�[�Q�b�g��ݒ�
	void SetTarget(const DirectX::XMFLOAT3& position, const float offset = 1.0f);

	// �o�H�T���̒T���ς݂�����
	void ClearNaviMeshSearch();

	// ���̃��b�V���ɖʂ��Ă��Ȃ����_�̎Z�o
	void GetNotContactPos(NaviMesh* now, NaviMesh* next, DirectX::XMFLOAT3& outPos);

	// ���̃��b�V���ɖʂ��Ă��钸�_�̎Z�o
	void GetContactPos(NaviMesh* now, NaviMesh* next, DirectX::XMFLOAT3* outPos);

	// �o�H�T��
	void SearchRoot(NaviMesh*& start, NaviMesh*& end);

	// �o�H����
	bool FindRoot(const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3& outPos, const float offset = 1.0f);

	// NaviMesh������ۂ̋���(�o�H)���Z�o
	bool NearLengthRoot(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3& outPos);

	// ���C�ɓ��������i�r���b�V���̎擾
	bool GetOnNaviMesh(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, NaviMesh*& mesh);

	NaviTarget* GetNaviTarget() { return target.get(); }

	static Navigation& Instance() { return *instance; }

private:

	// �n�ʃ��b�V�������蔻��
	std::vector<Node> naviNodes;

	// �i�r���b�V��
	std::vector<NaviMesh*> naviMeshes;

	// �n�_����I�_�܂ł̃G�b�W
	std::vector<NaviMesh*> rootMeshes;

	// �^�[�Q�b�g�̏��
	std::unique_ptr<NaviTarget> target;

	static Navigation* instance;

};