#include "UiManager.h"


void Ui::DebugRender()
{
	PP::Debug::Primitive2D::Box(position, size, center, angle, color);
}

void Ui::Register(Ui* child)
{
	child->parent = this; // e‚Ìİ’è
	children.emplace_back(child);
}

void Ui::Destroy()
{
	UiManager::Instance().Remove(this);
}

void Ui::Clear()
{
	for (Ui* ui : children)
	{
		if (ui != nullptr)
		{
			delete ui;
			ui = nullptr;
		}
	}
	children.clear();
}