#pragma once

#include "Ui.h"

class UiManager
{
public:

	UiManager();
	~UiManager();

	void Init();

	void Update(float elapsedTime);

	void Render(PP::RenderContext rc);

	void DebugRender();

	void Register(Ui* ui);

	void Remove(Ui* ui);
	
	void Clear();

	static UiManager& Instance() { return *instance; }

private:

	std::vector<Ui*> uies = {};
	std::vector<Ui*> removes = {};

	static UiManager* instance;

};