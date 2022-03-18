#include "SceneClear.h"
#include "SceneManager.h"
#include "SceneLoad.h"
#include "SceneTitle.h"

void SceneClear::Init()
{
	PP::Camera::SetLookAt({ 0, 10, -10 }, { 0, 0, 0 }, { 0, 1, 0 });
	PP::Camera::SetPerspective(
		PP::System::GetToRadian(45),
		(static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)),
		0.01f, 10000.0f);

	PP::Image::Load("Data/Image/Clear.png", sprite);

}

void SceneClear::Update(float elapsedTime)
{

	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_A)
	{
		SceneManager::Instance()->ChangeScene(new SceneLoad(new SceneTitle));
	}
}

void SceneClear::Render()
{
	PP::RenderContext rc;
	rc.view = PP::Camera::GetView();
	rc.projection = PP::Camera::GetProjection();

	PP::Image::Render({ 800, 450 },
		{ 1.0f, 1.0f },
		0, { 1.0f, 1.0f, 1.0f, 1.0f }, sprite);
}

void SceneClear::DebugRender()
{
	PP::Imgui::NewFrame();

	PP::RenderContext rc;
	rc.view = PP::Camera::GetView();
	rc.projection = PP::Camera::GetProjection();

	PP::Imgui::Render();
}

void SceneClear::End()
{

}