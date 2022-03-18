#pragma once

#include "Lib/Lib.h"

class Ui // 正直親子関係はいらないので後々削除予定
{
public:

	Ui() {}
	virtual ~Ui() {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(PP::RenderContext rc) = 0;

	virtual void DebugRender();

	void Register(Ui* child);

	void Destroy(); // 自分を消す

	void Clear(); // 子供を消す

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