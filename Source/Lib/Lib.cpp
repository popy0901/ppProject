#include "Lib.h"
#include "Graphics/Graphics.h"
#include "HighResolutionTimer.h"
#include "Framework.h"
#include "LibCamera.h"
#include "Easing.h"

#include <ctime>
#include <sstream>

#include "Input/Keyborad.h"

namespace PP
{
	struct Members
	{
		HWND hwnd = 0; // ウィンドウハンドル
		HighResolutionTimer		timer;

		Class::Framework*		f = nullptr;
		Audio*					audio = nullptr;
		Class::Graphics*		graphics = nullptr;
		Class::Input*			input = nullptr;
		Class::LibCamera*		camera = nullptr;
		Class::LibCollision*	collision = nullptr;
		Class::LibEasing* easing = nullptr;

	};
	Members m;

	// ライブラリの初期処理
	void Init(LPCTSTR caption, int width, int height, bool isFullscreen, double frameRate)
	{
		// 乱数系列の設定
		srand((unsigned int)time(NULL));

#ifdef _DEBUG
		// フルスクリーンの設定（念のためデバッグモードでは不可）
		isFullscreen = false;
#endif

		// ウィンドウの初期設定
		m.hwnd = Window::Init(caption, width, height);

		// DirectX11の初期設定
		DirectX11::Init(m.hwnd, width, height, isFullscreen);

		// フレームレートの設定
		m.timer.setFrameRate(frameRate);

	}

	// ライブラリの終了処理
	void End()
	{
		// DirectX11の終了処理
		DirectX11::End();

		// ウィンドウの終了処理
		Window::End();
	}

	template <typename T>
	inline void safe_delete(T*& p)
	{
		if (p != nullptr)
		{
			delete (p);
			(p) = nullptr;
		}
	}

	// ループ処理
	bool Loop(bool isShowFrameRate)
	{
		MSG msg = {};

		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return false;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Input::Update();

		//フレームレート用
		while (!m.timer.Tick());
		if (isShowFrameRate)
		{
			m.f->CalculateFrameStats(); //フレームレート計算・タイトルバーに表示
		}
		float elapsedTime = 1 == 0
			? m.timer.TimeInterval()
			: 1 / 60.0f;
		m.f->SetElapsedTime(elapsedTime);
		return true;
		//return static_cast<int>(msg.wParam);
	}



	namespace Window
	{

		LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			if (Imgui::HandleMessage(hwnd, msg, wparam, lparam))
				return true;

			switch (msg)
			{
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc;
				hdc = BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
				break;
			}
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_ACTIVATEAPP:
				//Keyboard::ProcessMessage(msg, wParam, lParam);
				//nWheelFraction = 0;
				break;
			case WM_KEYDOWN:
				if (wparam == VK_ESCAPE)
				{
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				}
			case WM_MOUSEWHEEL:
				m.input->GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wparam) / 120);
				break;
			case WM_SYSKEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYUP:
				//Keyboard::ProcessMessage(msg, wParam, lParam);
				break;
			case WM_ENTERSIZEMOVE:
				// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
				m.timer.Stop();
				break;
			case WM_EXITSIZEMOVE:
				// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
				// Here we reset everything based on the new window dimensions.
				m.timer.Start();
				break;
			default:
				return DefWindowProc(hwnd, msg, wparam, lparam);
			}

			return 0;
		}

		HWND Init(LPCTSTR caption, int width, int height)
		{
#if defined(DEBUG) | defined(_DEBUG)
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
			//_CrtSetBreakAlloc(30465);
#endif
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = fnWndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = GetModuleHandle(NULL);
			wcex.hIcon = 0;
			wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = NULL;
			wcex.lpszClassName = _T("3dgp");
			wcex.hIconSm = 0;
			RegisterClassEx(&wcex);

			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
			HWND hwnd = CreateWindow(_T("3dgp"), caption, WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, GetModuleHandle(NULL), NULL);
			ShowWindow(hwnd, SW_SHOWDEFAULT);

			return hwnd;
		}

		void End()
		{
			UnregisterClass(_T("3dgp"), GetModuleHandle(NULL));
		}
	}

	namespace DirectX11
	{
		HRESULT Init(HWND hwnd, int width, int height, bool isFullscreen)
		{
			HRESULT hr = S_OK;

			m.f = new Class::Framework(hwnd);

			m.graphics = new Class::Graphics(hwnd, isFullscreen);

			m.input = new Class::Input(hwnd);

			m.camera = new Class::LibCamera();

			m.collision = new Class::LibCollision();

			m.easing = new Class::LibEasing();

			return hr;
		}

		void End()
		{
			safe_delete(m.easing);
			safe_delete(m.camera);
			safe_delete(m.collision);
			safe_delete(m.f);
			safe_delete(m.input);
			safe_delete(m.graphics);
		}
	}

	namespace System
	{
		float GetElapsedTime()
		{
			return m.f->GetElapsedTime();
		}

		float GetRandF(float min, float max)
		{
			float r = 0.0f;
			r = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
			return r;
		}

		float GetToRadian(float angle)
		{
			return DirectX::XMConvertToRadians(angle);
		}

		float Lerp(float a, float b, float c)
		{
			return a * (1.0f - c) + (b * c);
		}
	}

	namespace Screen
	{
		void Clear(const DirectX::XMFLOAT4& color)
		{
			m.graphics->ScreenClear(color);
		}

		// 次のバックバッファの内容を表示
		HRESULT Flip(UINT SyncInterval, UINT Flags)
		{
			return m.graphics->GetSwapChain()->Present(SyncInterval, Flags);
		}

	}

	namespace Camera
	{
		void SetLookAt(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT3& target,
			const DirectX::XMFLOAT3& up)
		{
			m.camera->SetLookAt(pos, target, up);
		}

		void SetPerspective(float fovY, float aspect, float nearZ, float farZ)
		{
			m.camera->SetPerspective(fovY, aspect, nearZ, farZ);
		}

		const DirectX::XMFLOAT4X4& GetView()
		{
			return m.camera->GetView();
		}

		const DirectX::XMFLOAT4X4& GetProjection()
		{
			return m.camera->GetProjection();
		}

		const DirectX::XMFLOAT3& GetPos()
		{
			return m.camera->GetPos();
		}

		const DirectX::XMFLOAT3& GetTarget()
		{
			return m.camera->GetTarget();
		}

		const DirectX::XMFLOAT3& GetRight()
		{
			return m.camera->GetRight();
		}

		const DirectX::XMFLOAT3& GetUp()
		{
			return m.camera->GetUp();
		}

		const DirectX::XMFLOAT3& GetFront()
		{
			return m.camera->GetFront();
		}

	}

	namespace Imgui
	{
		LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (ImGui::GetCurrentContext() == NULL)
				return 0;

			ImGuiIO& io = ImGui::GetIO();
			switch (msg)
			{
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
			case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
			{
				int button = 0;
				if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
				if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
				if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
				if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
				if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
					::SetCapture(hWnd);
				io.MouseDown[button] = true;
				return 0;
			}
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			{
				int button = 0;
				if (msg == WM_LBUTTONUP) { button = 0; }
				if (msg == WM_RBUTTONUP) { button = 1; }
				if (msg == WM_MBUTTONUP) { button = 2; }
				if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
				io.MouseDown[button] = false;
				if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hWnd)
					::ReleaseCapture();
				return 0;
			}
			case WM_MOUSEWHEEL:
				io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				return 0;
			case WM_MOUSEHWHEEL:
				io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
				return 0;
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				if (wParam < 256)
					io.KeysDown[wParam] = 1;
				return 0;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				if (wParam < 256)
					io.KeysDown[wParam] = 0;
				return 0;
			case WM_CHAR:
				// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
				if (wParam > 0 && wParam < 0x10000)
					io.AddInputCharacterUTF16((unsigned short)wParam);
				return 0;
			case WM_SETCURSOR:
				if (LOWORD(lParam) == HTCLIENT/* && UpdateMouseCursor()*/)
					return 1;
				return 0;
			}
			return 0;
		}

		void NewFrame()
		{
			m.graphics->GetImGuiRenderer()->NewFrame();
		}

		void Render()
		{
			m.graphics->GetImGuiRenderer()->Render(m.graphics->GetDeviceContext());
		}
		void Begin(const char* name, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size)
		{
			ImGui::SetNextWindowPos({ pos.x, pos.y });
			ImGui::SetNextWindowSize({ size.x, size.y });
			ImGui::Begin(name, nullptr);
		}

		void End()
		{
			ImGui::End();
		}

		void Separator()
		{
			ImGui::Separator();
		}

		void SameLine()
		{
			ImGui::SameLine();
		}

		bool BeginMainMenuBar()
		{
			return ImGui::BeginMainMenuBar();
		}

		bool BeginMenu(const char* label)
		{
			return ImGui::BeginMenu(label);
		}

		bool MenuItem(const char* label, const char* shortcut, bool* select)
		{
			return ImGui::MenuItem(label, shortcut, select);
		}

		void EndMenu()
		{
			ImGui::EndMenu();
		}

		void EndMainMenuBar()
		{
			ImGui::EndMainMenuBar();
		}

		bool TreeNode(const char* label)
		{
			return ImGui::TreeNode(label);
		}

		void TreePop()
		{
			ImGui::TreePop();
		}

		void Text(const char* fmt, ...)
		{
			ImGui::Text(fmt);
		}

		bool Button(const char* label, DirectX::XMFLOAT2 size)
		{
			return ImGui::Button(label, { size.x, size.y });
		}

		bool CheckBox(const char* label, bool* v)
		{
			return ImGui::Checkbox(label, v);
		}

		bool RadioButton(const char* label, int* v, int button)
		{
			return ImGui::RadioButton(label, v, button);
		}

		bool SlidInt(const char* label, int* v, int min, int max)
		{
			return ImGui::SliderInt(label, v, min, max);
		}

		bool SlidFloat(const char* label, float* v, float min, float max)
		{
			return ImGui::SliderFloat(label, v, min, max);
		}
		bool SlidFloat2(const char* label, float* v, float min, float max)
		{
			return ImGui::SliderFloat2(label, v, min, max);
		}
		bool SlidFloat3(const char* label, float* v, float min, float max)
		{
			return ImGui::SliderFloat3(label, v, min, max);
		}
		bool SlidFloat4(const char* label, float* v, float min, float max)
		{
			return ImGui::SliderFloat4(label, v, min, max);
		}

		bool InputInt(const char* label, int* v)
		{
			return ImGui::InputInt(label, v);
		}

		bool InputFloat(const char* label, float* v)
		{
			return ImGui::InputFloat(label, v);
		}
		bool InputFloat2(const char* label, float* v)
		{
			return ImGui::InputFloat2(label, v);
		}
		bool InputFloat3(const char* label, float* v)
		{
			return ImGui::InputFloat3(label, v);
		}
		bool InputFloat4(const char* label, float* v)
		{
			return ImGui::InputFloat4(label, v);
		}

		void TextColor(DirectX::XMFLOAT3 color, const char* text)
		{
			ImGui::TextColored(ImVec4(color.x, color.y, color.z, 1.0f), text);
		}

		bool ColorEdit(const char* label, DirectX::XMFLOAT3 color)
		{
			return ImGui::ColorEdit3(label, (float*)&color);
		}

		void Test()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					bool check = false;
					if (ImGui::MenuItem("Open Model", "", &check))
					{
						//OpenModelFile();
					}

					if (ImGui::MenuItem("Open Animation", "", &check))
					{
						//OpenAnimationFile();
					}

					if (ImGui::MenuItem("Save Model", "", &check))
					{
						//SaveModelFile();
					}

					ImGui::EndMenu();
				}
			}
			ImGui::EndMainMenuBar();
		}
	}

	namespace Debug
	{
		namespace Primitive3D
		{
			void Render(const RenderContext& rc)
			{
				m.graphics->GetDebugRenderer()->Render(m.graphics->GetDeviceContext(), rc.view, rc.projection);
				m.graphics->GetLineRenderer()->Render(m.graphics->GetDeviceContext(), rc.view, rc.projection);
			}

			void DrawSphere(const DirectX::XMFLOAT3& pos, float radius, const DirectX::XMFLOAT4& color)
			{
				m.graphics->GetDebugRenderer()->DrawSphere(pos, radius, color);
			}

			void DrawCylinder(const DirectX::XMFLOAT3& pos, float radius, float height, const DirectX::XMFLOAT4& color)
			{
				m.graphics->GetDebugRenderer()->DrawCylinder(pos, radius, height, color);
			}

			void DrawLine(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT4& color)
			{
				m.graphics->GetLineRenderer()->DrawLine(pos1, pos2, color);
			}

			void DrawCube(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT4& color)
			{
				m.graphics->GetLineRenderer()->DrawCube(pos1, pos2, color);
			}
		}

		namespace Primitive2D
		{
			void Box(const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT2& center, float angle, const DirectX::XMFLOAT4& color)
			{
				m.graphics->GetPrimitive2d()->Rect(m.graphics->GetDeviceContext(), pos, size, center, angle, color);
			}

			void Circle(const DirectX::XMFLOAT2& pos, float radius, const DirectX::XMFLOAT4& color)
			{
				m.graphics->GetPrimitive2d()->Circle(m.graphics->GetDeviceContext(), pos, radius, color, 32);
			}

			void Line(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2, const DirectX::XMFLOAT4& color, int width)
			{
				m.graphics->GetPrimitive2d()->Line(m.graphics->GetDeviceContext(), pos1, pos2, color, width);
			}
		}
	}

	namespace Shader
	{
		ppShader Lambert()
		{
			return m.graphics->GetLambert();
		}

		ppShader Destraction()
		{
			return m.graphics->GetDestraction();
		}

		ppShader DepthShadow()
		{
			return m.graphics->GetDepthShadow();
		}

		ppShader RenderShadow()
		{
			return m.graphics->GetRenderShadw();
		}

		namespace Paramter
		{
			void SetDestraction(float scale, float angle, float position)
			{
				m.graphics->SetDestractionParamter(scale, angle, position);
			}

			void SetShadowShader(const DirectX::XMFLOAT3& target)
			{
				m.graphics->SetShadowTarget(target);
			}
		}
	}

	namespace Image
	{
		void Load(const char* fileName, ppImage& sp)
		{
			sp = std::make_shared<Class::Sprite>(fileName);
		}

		void Render(const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& scale, float angle, const DirectX::XMFLOAT4& color, ppImage& sp)
		{
			sp->Render(m.graphics->GetDeviceContext(),
				pos.x - sp->GetTextureWidth() / 2 * scale.x, pos.y - sp->GetTextureHeight() / 2 * scale.y, 
				sp->GetTextureWidth() * scale.x, sp->GetTextureHeight() * scale.y, // スクリーン座標
				0, 0, static_cast<float>(sp->GetTextureWidth()), static_cast<float>(sp->GetTextureHeight()), // テクスチャ座標
				angle,
				color.x, color.y, color.z, color.w);
		}
	}

	namespace Model
	{
		/*void Load(const char* fileName, ppModel& model)
		{
			model = std::make_shared<Class::LibModel>(fileName);
		}*/

		void Load(const char* fileName, std::string id)
		{
			Class::LibCopyModel::Instance()->Load(fileName, id);
		}

		void Make(ppModel& model, std::string id)
		{
			model = std::make_shared<Class::LibModel>(id);
		}

		/*void Release(std::string id)
		{
			Class::LibCopyModel::Instance()->Release(id);
		}*/

		ppNode FindNode(ppModel& model, const char* name)
		{
			return model->FindNode(name);
		}

		const DirectX::XMFLOAT3 GetNodePosition(ppNode& node)
		{
			return { node->worldTransform._41, node->worldTransform._42, node->worldTransform._43 };
		}

		void UpdateTransform(ppModel& model, const DirectX::XMFLOAT4X4& transform)
		{
			model->UpdateTransform(transform);
		}

		void UpdateTransform(ppModel& model, const WorldContext& wc)
		{
			model->UpdateTransform(wc);
		}

		const DirectX::XMFLOAT3 GetForward(ppModel& model)
		{
			return model->GetForward();
		}

		const DirectX::XMFLOAT4X4 GetTransform(ppModel& model)
		{
			return model->GetTransform();
		}

		void UpdateAnimation(ppModel& model, float elapsedTime)
		{
			model->UpdateAnimation(elapsedTime);
		}

		void PlayAnimation(ppModel& model, int index, bool loop, float blendSeconds)
		{
			model->PlayAnimation(index, loop, blendSeconds);
		}

		bool IsPlayAnimation(ppModel& model)
		{
			return model->IsPlayAnimation();
		}

		void UpdateSubAnimation(ppModel& model, float elapsedTime)
		{
			model->UpdateSubAnimation(elapsedTime);
		}

		void PlaySubAnimation(ppModel& model, const char* subNodeName, int index, bool loop, float blendSeconds)
		{
			model->PlaySubAnimation(subNodeName, index, loop, blendSeconds);
		}

		void StopSubAnimation(ppModel& model)
		{
			model->StopSubAnimation();
		}

		bool IsPlaySubAnimation(ppModel& model)
		{
			return model->IsPlaySubAnimation();
		}

		void SetCurrentAnimationSeconds(ppModel& model, float currentAnimationSeconds)
		{
			model->SetCurrentAnimationSeconds(currentAnimationSeconds);
		}

		float GetCurrentAnimationSeconds(ppModel& model)
		{
			return model->GetCurrentAnimationSeconds();
		}

		void Begin(const RenderContext& rc, ppShader shader)
		{
			shader->Begin(m.graphics->GetDeviceContext(), rc);
		}

		void Render(ppModel& model, ppShader shader)
		{
			shader->Draw(m.graphics->GetDeviceContext(), model.get());
		}

		void End(ppShader shader)
		{
			shader->End(m.graphics->GetDeviceContext());
		}
	}

	namespace Effect
	{
		void Load(const char* fileName, ppEffect& pfc)
		{
			pfc = std::make_shared<Class::LibEfc>(fileName);
		}

		void Play(ppEffect& pfc, DirectX::XMFLOAT3 pos)
		{
			pfc->Play(pos);
		}

		void Release(ppEffect& pfc)
		{
			pfc->Release();
		}

		void Update(ppEffect& pfc, float elapsedTime)
		{
			pfc->Update(elapsedTime);
		}

		void Render(ppEffect& pfc, const RenderContext& rc)
		{
			pfc->Render(m.graphics->GetDeviceContext(), rc);
		}

		void SetPosition(ppEffect& pfc, DirectX::XMFLOAT3 position)
		{
			pfc->SetPosition(position);
		}
	}

	namespace Light
	{
		void SetPosition(const DirectX::XMFLOAT3& position)
		{
			m.graphics->GetLight()->SetPosition(position);
		}
		void SetDirection(const DirectX::XMFLOAT4& direction)
		{
			m.graphics->GetLight()->SetDirection(direction);
		}

		void SetColor(const DirectX::XMFLOAT4& color)
		{
			m.graphics->GetLight()->SetColor(color);
		}

		void SetAmbient(const DirectX::XMFLOAT4& ambient)
		{
			m.graphics->GetLight()->SetAmbient(ambient);
		}
	}

	namespace Collision
	{
		bool SphereVsSphere(const DirectX::XMFLOAT3& position1,
			float radius1,
			const DirectX::XMFLOAT3& position2,
			float radius2,
			DirectX::XMFLOAT3& outPosition2)
		{
			return m.collision->SphereVsSphere(position1, radius1, 
				position2, radius2, outPosition2);
		}

		bool CylinderVsCylinder(
			const DirectX::XMFLOAT3& position1,
			float radius1,
			float height1,
			const DirectX::XMFLOAT3& position2,
			float radius2,
			float height2,
			DirectX::XMFLOAT3& outPosition2)
		{
			return m.collision->CylinderVsCylinder(position1, radius1, height1, 
				position2, radius2, height2, outPosition2);
		}

		bool SphereVsCylinder(
			const DirectX::XMFLOAT3& spherePos,
			float sphereRadius,
			const DirectX::XMFLOAT3& cylinderpos,
			float cylinderRadius,
			float cylinderHeight,
			DirectX::XMFLOAT3 outCylinderPos)
		{
			return m.collision->IntersectSphereVsCylinder(
				spherePos, sphereRadius,
				cylinderpos, cylinderRadius, cylinderHeight,
				outCylinderPos);
		}

		bool RayCast(ppModel& model,
			const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end,
			PP::HitResult& hit)
		{
			return m.collision->IntersectRayVsMode(start, end, model.get(), hit);
		}
	}

	namespace Blend
	{

	}

	namespace Input
	{
		void Update()
		{
			m.input->Update();
		}

		int CheckHitKey(int code)
		{
			//static int push = false;
			//if (m.input->GetKeyBoard().GetKeyInput(code) > 0)
			//{
			//	push = true;
			//}
			//if (push)
			//	if (m.input->GetKeyBoard().GetKeyInput(code) == 0)
			//	{
			//		push = false;
			//		return -1;
			//	}
			//else return m.input->GetKeyBoard().GetKeyInput(code);

			return m.input->GetKeyBoard().GetKeyInput(code);
		}

		const DirectX::XMFLOAT2& GetMousePos()
		{
			return m.input->GetMouse().GetPosition();
		}

		int GetWheel()
		{
			return m.input->GetMouse().GetWheel();
		}

		int GetDeltaWheel()
		{
			return m.input->GetMouse().GetDeltaWheel();
		}

		float GetLeftStickX()
		{
			return m.input->GetGamePad().GetAxisLX();
		}

		float GetLeftStickY()
		{
			return m.input->GetGamePad().GetAxisLY();
		}

		float GetRightStickX()
		{
			return m.input->GetGamePad().GetAxisRX();
		}

		float GetRightStickY()
		{
			return m.input->GetGamePad().GetAxisRY();
		}

	}

	namespace Easing
	{
		float EaseInSine(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInSine(a, b, timer, totalTime);
		}

		float EaseOutSine(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutSine(a, b, timer, totalTime);
		}

		float EaseInOutSine(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutSine(a, b, timer, totalTime);
		}

		float EaseInQuad(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInQuad(a, b, timer, totalTime);
		}

		float EaseOutQuad(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutQuad(a, b, timer, totalTime);
		}

		float EaseInOutQuad(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutQuad(a, b, timer, totalTime);
		}

		float EaseInCubic(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInCubic(a, b, timer, totalTime);
		}

		float EaseOutCubic(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutCubic(a, b, timer, totalTime);
		}

		float EaseInOutCubic(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutCubic(a, b, timer, totalTime);
		}

		float EaseInQuart(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInQuart(a, b, timer, totalTime);
		}

		float EaseOutQuart(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutQuart(a, b, timer, totalTime);
		}

		float EaseInOutQuart(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutQuart(a, b, timer, totalTime);
		}

		float EaseInQuint(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInQuint(a, b, timer, totalTime);
		}

		float EaseOutQuint(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutQuint(a, b, timer, totalTime);
		}

		float EaseInOutQuint(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutQuint(a, b, timer, totalTime);
		}

		float EaseInExpo(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInExpo(a, b, timer, totalTime);
		}

		float EaseOutExpo(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutExpo(a, b, timer, totalTime);
		}

		float EaseInOutExpo(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutExpo(a, b, timer, totalTime);
		}

		float EaseInCirc(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInCirc(a, b, timer, totalTime);
		}

		float EaseOutCirc(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutCirc(a, b, timer, totalTime);
		}

		float EaseInOutCirc(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutCirc(a, b, timer, totalTime);
		}

		float EaseInBack(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInBack(a, b, timer, totalTime);
		}

		float EaseOutBack(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutBack(a, b, timer, totalTime);
		}

		float EaseInOutBack(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutBack(a, b, timer, totalTime);
		}

		float EaseInElastic(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInElastic(a, b, timer, totalTime);
		}

		float EaseOutElastic(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutElastic(a, b, timer, totalTime);
		}

		float EaseInOutElastic(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutElastic(a, b, timer, totalTime);
		}

		float EaseInBounce(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInBounce(a, b, timer, totalTime);
		}

		float EaseOutBounce(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseOutBounce(a, b, timer, totalTime);
		}

		float EaseInOutBounce(float a, float b, float timer, float totalTime)
		{
			return m.easing->EaseInOutBounce(a, b, timer, totalTime);
		}
	}

}