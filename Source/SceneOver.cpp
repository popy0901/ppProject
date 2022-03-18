#include "SceneOver.h"
#include "SceneManager.h"
#include "SceneLoad.h"
#include "SceneTitle.h"

void SceneOver::Init()
{
	PP::Camera::SetLookAt({ 0, 10, -10 }, { 0, 0, 0 }, { 0, 1, 0 });
	PP::Camera::SetPerspective(
		PP::System::GetToRadian(45),
		(static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)),
		0.01f, 10000.0f);

	PP::Image::Load("Data/Image/Over.png", sprite);

}

void SceneOver::Update(float elapsedTime)
{


	GamePad& pad = PP::Class::Input::Instance().GetGamePad();

	if (pad.GetButtonDown() & GamePad::BTN_A)
	{
		SceneManager::Instance()->ChangeScene(new SceneLoad(new SceneTitle));
	}
}

void SceneOver::Render()
{
	PP::RenderContext rc;
	rc.view = PP::Camera::GetView();
	rc.projection = PP::Camera::GetProjection();

	PP::Image::Render({ 800, 450 },
		{ 1.0f, 1.0f },
		0, { 1.0f, 1.0f, 1.0f, 1.0f }, sprite);
}

void SceneOver::DebugRender()
{
	PP::Imgui::NewFrame();

	PP::RenderContext rc;
	rc.view = PP::Camera::GetView();
	rc.projection = PP::Camera::GetProjection();

	PP::Imgui::Render();
}

void SceneOver::End()
{

}