#pragma once
#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>

#include "Audio/Audio.h"
#include "Graphics/RenderContext.h"
#include "Graphics/Sprite.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Pfc.h"
#include "LibCollision.h"
#include "Input/Input.h"

const LONG SCREEN_WIDTH = 1600;
const LONG SCREEN_HEIGHT = 900;


namespace PP
{
	// ���C�u�����̏�������
	void Init(LPCTSTR caption, int width = (1600), int height = (900),
		bool isFullscreen = (false), double frameRate = (60.0));

	// ���C�u�����̏I������
	void End();

	// ���[�v����
	bool Loop(bool isShowFrameRate = false);

	namespace Window
	{
		// �E�B���h�E�̏����ݒ�
		HWND Init(LPCTSTR caption, int width, int height);

		// �E�B���h�E�̏I������
		void End();
	}

	namespace DirectX11
	{
		// DirectX�̏�������
		HRESULT Init(HWND hwnd, int width, int height, bool isFullScreen);

		// DirectX�̏I������
		void End();
	}

	namespace System
	{
		float GetElapsedTime(); // �t���[���擾

		float GetRandF(float min, float max); //min �` max �͈̔͂̃����_���l��Ԃ�

		float GetToRadian(float angle); // ���W�A���p

		float Lerp(float a, float b, float c);
	}

	namespace Screen
	{
		// ��ʃN���A
		void Clear(const DirectX::XMFLOAT4& = { 0, 0, 0, 0 });

		// ��ʃt���b�v
		// �������͐��������̃^�C�~���O(0�Ȃ瑦���Ή�, ��0�Ȃ琔�l�̎��Ԍo�ߌ�ɑΉ�)
		HRESULT Flip(UINT SyncInterval = 1, UINT Flags = 0);
	}

	namespace Camera
	{
		void SetLookAt(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT3& target,
			const DirectX::XMFLOAT3& up);

		void SetPerspective(float fovY, float aspect, float nearZ, float farZ);

		const DirectX::XMFLOAT4X4& GetView();

		const DirectX::XMFLOAT4X4& GetProjection();

		const DirectX::XMFLOAT3& GetPos();

		const DirectX::XMFLOAT3& GetTarget();

		const DirectX::XMFLOAT3& GetRight();

		const DirectX::XMFLOAT3& GetUp();

		const DirectX::XMFLOAT3& GetFront();
	}

	namespace Imgui
	{
		LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void Test();

		void NewFrame(); // �t���[���̍X�V

		void Render(); // �`��

		// ���n��
		void Begin(const char* name, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);

		// ���I���
		void End();

		void Separator(); // ������

		void SameLine(); // ������

		bool BeginMainMenuBar();

		bool BeginMenu(const char* label);

		bool MenuItem(const char* label, const char* shortcut, bool* select);

		void EndMenu();

		void EndMainMenuBar();

		bool TreeNode(const char* label);

		void TreePop();

		void Text(const char* fmt, ...); // ����

		bool Button(const char* label, DirectX::XMFLOAT2 size = { 0.0f, 0.0f });

		bool CheckBox(const char* label, bool* v);

		bool RadioButton(const char* label, int* v, int button);

		bool SlidInt(const char* label, int* v, int min, int max);

		bool SlidFloat(const char* label, float* v, float min, float max);
		bool SlidFloat2(const char* label, float* v, float min, float max);
		bool SlidFloat3(const char* label, float* v, float min, float max);
		bool SlidFloat4(const char* label, float* v, float min, float max);

		bool InputInt(const char* label, int* v);

		bool InputFloat(const char* label, float* v);
		bool InputFloat2(const char* label, float* v);
		bool InputFloat3(const char* label, float* v);
		bool InputFloat4(const char* label, float* v);

		void TextColor(DirectX::XMFLOAT3 color, const char* text);

		bool ColorEdit(const char* label, DirectX::XMFLOAT3 color);

	}

	namespace Debug
	{
		namespace Primitive3D
		{
			void Render(const RenderContext& rc);

			void DrawSphere(const DirectX::XMFLOAT3& pos, float radius, const DirectX::XMFLOAT4& color);

			void DrawCylinder(const DirectX::XMFLOAT3& pos, float radius, float height, const DirectX::XMFLOAT4& color);

			void DrawLine(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT4& color);

			void DrawCube(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT4& color);
		}
		namespace Primitive2D
		{
			void Box(const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT2& center, float angle, const DirectX::XMFLOAT4& color);

			void Circle(const DirectX::XMFLOAT2& pos, float radius, const DirectX::XMFLOAT4& color);

			void Line(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2, const DirectX::XMFLOAT4& color, int width = 1);
		}

	}

	using ppShader = Class::LibShader*;
	namespace Shader
	{
		ppShader Lambert();
		ppShader Destraction();
		ppShader DepthShadow();
		ppShader RenderShadow();
		namespace Paramter
		{
			void SetDestraction(float scale, float angle, float position);
			void SetShadowShader(const DirectX::XMFLOAT3& target);
		}
	}

	using ppImage = std::shared_ptr<Class::Sprite>; // 2D�摜�n���h��
	namespace Image
	{
		void Load(const char* fileName, ppImage& sp);
		void Render(const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& scale, float angle, const DirectX::XMFLOAT4& color, ppImage& sp);
	}

	using ppModel = std::shared_ptr<Class::LibModel>; // 3D���f���n���h��
	using ppNode = Class::LibModel::Node*; // ���f���̃m�[�h(�{�[��)
	namespace Model
	{
		// ���f���ǂݍ���(mdl)
		//void Load(const char* fileName, ppModel& model);

		// ���f���̓ǂݍ���
		void Load(const char* fileName, std::string id); // ID�ݒ�

		// �ǂ݂��񂾃��f���̍쐬
		void Make(ppModel& model, std::string id); // ID�ݒ�

		// ��������(���Ԃ�Ȃ��Ă����v)
		//void Release(std::string id);

		// �m�[�h����
		ppNode FindNode(ppModel& model, const char* name);

		// �m�[�h�̍��W�擾
		const DirectX::XMFLOAT3 GetNodePosition(ppNode& node);

		// �s��X�V
		void UpdateTransform(ppModel& model, const DirectX::XMFLOAT4X4& transform);

		// �s��X�V
		void UpdateTransform(ppModel& model, const WorldContext& wc);

		// �O�x�N�g���擾
		const DirectX::XMFLOAT3 GetForward(ppModel& model);

		// �s��擾
		const DirectX::XMFLOAT4X4 GetTransform(ppModel);

		// �A�j���[�V�����X�V
		void UpdateAnimation(ppModel& model, float elapsedTime);

		// �A�j���[�V�����Đ�
		void PlayAnimation(ppModel& model, int index, bool loop = false, float blendSeconds = 0.2f);

		// �A�j���[�V��������
		bool IsPlayAnimation(ppModel& model);

		void UpdateSubAnimation(ppModel& model, float elapsedTime);

		void PlaySubAnimation(ppModel& model, const char* subNodeName, int index, bool loop, float blendSeconds = 0.2f);

		void StopSubAnimation(ppModel& model);

		bool IsPlaySubAnimation(ppModel& model);

		// �A�j���[�V�����Đ����Ԑݒ�
		void SetCurrentAnimationSeconds(ppModel& model, float currentAnimationSeconds);

		// �A�j���[�V�����Đ����Ԏ擾
		float GetCurrentAnimationSeconds(ppModel& model);

		// �`��J�n
		void Begin(const RenderContext& rc, ppShader shader);

		// �`��
		void Render(ppModel& model, ppShader shader);

		// �`��I��
		void End(ppShader shader);
	}

	using ppEffect = std::shared_ptr<Class::LibEfc>;
	namespace Effect
	{
		// �ǂݍ���(pfc)
		void Load(const char* fileName, ppEffect& pfc);

		// �Đ�(���������ʒu)
		void Play(ppEffect& pfc, DirectX::XMFLOAT3 pos = { 0, 0, 0 });

		// ����
		void Release(ppEffect& pfc);

		// �X�V
		void Update(ppEffect& pfc, float elapsedTime);

		// �`��
		void Render(ppEffect& pfc, const RenderContext& rc);

		// ���W�ݒ�(�X�V)
		void SetPosition(ppEffect& pfc, DirectX::XMFLOAT3 position);

		// �`��̎���]?(��肩���킩���)
		// �x�N�g���ړ�?(��肩���킩���)

	}

	namespace Light
	{
		void SetPosition(const DirectX::XMFLOAT3& position);

		void SetDirection(const DirectX::XMFLOAT4& direction);

		void SetColor(const DirectX::XMFLOAT4& color);

		void SetAmbient(const DirectX::XMFLOAT4& ambient);
	}

	namespace Collision
	{
		bool SphereVsSphere(const DirectX::XMFLOAT3& position1,
			float radius1,
			const DirectX::XMFLOAT3& position2,
			float radius2,
			DirectX::XMFLOAT3& outPosition2);

		bool CylinderVsCylinder(
			const DirectX::XMFLOAT3& position1,
			float radius1,
			float height1,
			const DirectX::XMFLOAT3& position2,
			float radius2,
			float height2,
			DirectX::XMFLOAT3& outPosition2);

		bool SphereVsCylinder(
			const DirectX::XMFLOAT3& spherePos,
			float sphereRadius,
			const DirectX::XMFLOAT3& cylinderpos,
			float cylinderRadius,
			float cylinderHeight,
			DirectX::XMFLOAT3 outCylinderPos);

		bool RayCast(ppModel& model,
			const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end,
			PP::HitResult& hit);
	}

	namespace Blend
	{

	}

	namespace Input
	{
		void Update();

		int CheckHitKey(int code); // 1 �������u��; // -1 �������u��; // > 0 �����Ă��

		const DirectX::XMFLOAT2& GetMousePos();

		const DirectX::XMFLOAT2& GetMouseDeltaPos();

		int GetWheel();

		int GetDeltaWheel();

		float GetLeftStickX();

		float GetLeftStickY();

		float GetRightStickX();

		float GetRightStickY();
	}

	namespace Easing
	{
		float EaseInSine(float a, float b, float timer, float totalTime);

		float EaseOutSine(float a, float b, float timer, float totalTime);

		float EaseInOutSine(float a, float b, float timer, float totalTime);

		float EaseInQuad(float a, float b, float timer, float totalTime);

		float EaseOutQuad(float a, float b, float timer, float totalTime);

		float EaseInOutQuad(float a, float b, float timer, float totalTime);

		float EaseInCubic(float a, float b, float timer, float totalTime);

		float EaseOutCubic(float a, float b, float timer, float totalTime);

		float EaseInOutCubic(float a, float b, float timer, float totalTime);

		float EaseInQuart(float a, float b, float timer, float totalTime);

		float EaseOutQuart(float a, float b, float timer, float totalTime);

		float EaseInOutQuart(float a, float b, float timer, float totalTime);

		float EaseInQuint(float a, float b, float timer, float totalTime);

		float EaseOutQuint(float a, float b, float timer, float totalTime);

		float EaseInOutQuint(float a, float b, float timer, float totalTime);

		float EaseInExpo(float a, float b, float timer, float totalTime);

		float EaseOutExpo(float a, float b, float timer, float totalTime);

		float EaseInOutExpo(float a, float b, float timer, float totalTime);

		float EaseInCirc(float a, float b, float timer, float totalTime);

		float EaseOutCirc(float a, float b, float timer, float totalTime);

		float EaseInOutCirc(float a, float b, float timer, float totalTime);

		float EaseInBack(float a, float b, float timer, float totalTime);

		float EaseOutBack(float a, float b, float timer, float totalTime);

		float EaseInOutBack(float a, float b, float timer, float totalTime);

		float EaseInElastic(float a, float b, float timer, float totalTime);

		float EaseOutElastic(float a, float b, float timer, float totalTime);

		float EaseInOutElastic(float a, float b, float timer, float totalTime);

		float EaseInBounce(float a, float b, float timer, float totalTime);

		float EaseOutBounce(float a, float b, float timer, float totalTime);

		float EaseInOutBounce(float a, float b, float timer, float totalTime);
	}

}