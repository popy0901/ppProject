#include "SceneGame.h"
#include "SceneManager.h" 
#include "SceneLoad.h"
#include "SceneClear.h"
#include "SceneOver.h"
#include "Lib/Lib.h"
#include "PlayerManager.h"
#include "StageManager.h"
#include "EnemyManager.h"
#include "Shot.h"
//#include "EffectManager.h"

// 初期化
void SceneGame::Init()
{
	PP::Camera::SetLookAt({ 0, 1, -10 }, { 0, 0, 0 }, { 0, 1, 0 });
	PP::Camera::SetPerspective(
		PP::System::GetToRadian(45),
		(static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)), 
		0.01f, 10000.0f);

	//EffectManager::Instance().Init(); // エフェクト初期化処理
	StageManager::Instance().Init();

	PlayerManager::Instance().Init();
	EnemyManager::Instance().Init();
	//ShotManager::Instance()->Init();

	PP::Model::Load("Data/Model/Z_test/Skydome/sky1.mdl", "SKY");
	sky = new Sky();

	camera = new Camera();

	uiManager = new UiManager();

	stageObjectManager = new StageObjectManager();

	meta = new Meta(&PlayerManager::Instance().GetPlayer(), &EnemyManager::Instance());

}
bool d = false; // ImGui表示切り替え
// 更新処理
void SceneGame::Update(float elapsedTime)
{
	camera->Update(elapsedTime);

	StageManager::Instance().Update(elapsedTime);
	sky->Update(elapsedTime);
	stageObjectManager->Update(elapsedTime);
	EnemyManager::Instance().Update(elapsedTime); // 敵更新↓との順番大事
	PlayerManager::Instance().Update(elapsedTime); // プレイヤー更新
	//ShotManager::Instance()->Update(elapsedTime);

	//EffectManager::Instance().Update(elapsedTime);
	
	uiManager->Update(elapsedTime);

	if (PP::Input::CheckHitKey('P') == 1) d ^= true;

	// クリア、オーバーへ遷移
	switch (SceneManager::Instance()->GetChangeType())
	{
	case SceneManager::CLEAR:
		SceneManager::Instance()->ChangeScene(new SceneLoad(new SceneClear));
		break;
	case SceneManager::OVER:
		SceneManager::Instance()->ChangeScene(new SceneLoad(new SceneOver));
		break;
	}

}

// 描画処理
void SceneGame::Render()
{
	PP::RenderContext rc;
	rc.view = PP::Camera::GetView();
	rc.projection = PP::Camera::GetProjection();

	// ライト 
	{
		//if (PP::Input::CheckHitKey('I')) pos.z += 0.3f;
		//if (PP::Input::CheckHitKey('J')) pos.x -= 0.3f;
		//if (PP::Input::CheckHitKey('K')) pos.z -= 0.3f;
		//if (PP::Input::CheckHitKey('L')) pos.x += 0.3f;
		//if (PP::Input::CheckHitKey('U')) pos.y -= 0.3f;
		//if (PP::Input::CheckHitKey('O')) pos.y += 0.3f;

		lightPosition.x = PlayerManager::Instance().GetPlayer().GetPosition().x - 30;
		lightPosition.z = PlayerManager::Instance().GetPlayer().GetPosition().z - 30;
		PP::Light::SetPosition(lightPosition);
		PP::Light::SetDirection(lightDirection); // ライト方向
		PP::Shader::Paramter::SetShadowShader(PlayerManager::Instance().GetPlayer().GetPosition());
	}

	// 3Dモデル描画
	{
		PP::Model::Begin(rc, PP::Shader::Lambert());
		sky->Render();
		PP::Model::End(PP::Shader::Lambert());

		PP::Model::Begin(rc, PP::Shader::DepthShadow());
		StageManager::Instance().RenderSub();
		//ShotManager::Instance()->RenderSub();
		stageObjectManager->RenderSub();
		EnemyManager::Instance().RenderSub();
		PlayerManager::Instance().RenderSub();
		PP::Model::End(PP::Shader::DepthShadow());

		PP::Model::Begin(rc, PP::Shader::RenderShadow());
		StageManager::Instance().Render();
		//ShotManager::Instance()->Render();
		stageObjectManager->Render();
		EnemyManager::Instance().Render();
		PlayerManager::Instance().Render();
		PP::Model::End(PP::Shader::RenderShadow());
	}

	// パーティクル
	{
		//EffectManager::Instance().Render(rc);
	}

	// 2Dスプライト描画
	{
		//static int b = 0;
		//b++;
		//PP::Image::Render({ 800, 450 }, 
		//	{ 1.0f, 1.0f }, 
		//	0, { 1, 1, 0, 0.5f }, sprite);
		uiManager->Render(rc);
	}

	
}

void SceneGame::DebugRender()
{
	if (d) return;
	PP::Imgui::NewFrame();

	PP::RenderContext rc;
	rc.view = PP::Camera::GetView();
	rc.projection = PP::Camera::GetProjection();

	// 3Dデバッグ描画
	{
		PP::Debug::Primitive3D::Render(rc);

		// 太陽
		PP::Debug::Primitive3D::DrawSphere(lightPosition, 1, { 1, 0.5f, 0, 1 });
		
		//ShotManager::Instance()->DebugRender();
		{
			sky->DebugRender();
			StageManager::Instance().DebugRender();
			stageObjectManager->DebugRender();
			EnemyManager::Instance().DebugRender();
			PlayerManager::Instance().DebugRender();
		}

		camera->DebugRender();
		
		// Light Debug
		{
			PP::Imgui::Begin(u8"Light", { 1600 - 256, 900 - 256 }, { 256, 256 });
			PP::Imgui::InputFloat3(u8"pos", &lightPosition.x);
			PP::Imgui::InputFloat4(u8"dir", &lightDirection.x);
			PP::Imgui::End();
		}
	}

	// 2DデバッグGUI描画
	{
		//static float c = 0;
		//c+= 0.1f;
		//PP::Debug::Primitive2D::Box({ 128.0f, 128.0f }, { 256.0f, 256.0f }, { 128.0f, 128.0f }, c, { 1.0f, 1.0f, 1.0f, 0.5f });
		//PP::Debug::Primitive2D::Circle({ 128.0f, 128.0f }, 16.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
		//PP::Debug::Primitive2D::Line({ 0.0f, 0.0f }, { 1600.0f, 900.0f }, { 1.0f, 0.0f, 1.0f, 1.0f });

		uiManager->DebugRender();
	}

	PP::Imgui::Render();

}

// 終了化
void SceneGame::End()
{
	delete meta;
	delete sky;
	delete uiManager;
	delete camera;
	delete stageObjectManager;
	StageManager::Instance().Clear();
	//EffectManager::Instance().Clear();
	EnemyManager::Instance().Clear();
	PlayerManager::Instance().Clear();
}