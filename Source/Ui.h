#pragma once

#include "Lib/Lib.h"

class Ui // �����e�q�֌W�͂���Ȃ��̂Ō�X�폜�\��
{
public:

	Ui() {}
	virtual ~Ui() {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(PP::RenderContext rc) = 0;

	virtual void DebugRender();

	void Register(Ui* child);

	void Destroy(); // ����������

	void Clear(); // �q��������

	void SetPos(DirectX::XMFLOAT2 pos)
	{
		position = pos;
	}

	const DirectX::XMFLOAT2& GetPosition() { return position; }

protected:

	DirectX::XMFLOAT2 position = {};
	DirectX::XMFLOAT2 size = {};
	DirectX::XMFLOAT2 center = {};
	DirectX::XMFLOAT4 color = {};

	float angle = 0.0f;
	float point = 0.0f;

	Ui* parent = nullptr;
	std::vector<Ui*> children = {};

};