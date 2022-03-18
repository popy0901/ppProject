#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Lib/Graphics/ModelResource.h"
#include "Lib/Graphics/WorldContext.h"

namespace PP
{
	namespace Class
	{
		// ���f��
		class LibModel
		{
		public:
			LibModel(std::string id);
			~LibModel() {}

			struct Node // �m�[�h(�{�[��)
			{
				int Index = 0;
				const char* name = nullptr;
				bool flag = false;
				Node* parent = nullptr;
				DirectX::XMFLOAT3	scale = {};
				DirectX::XMFLOAT4	rotate = {};
				DirectX::XMFLOAT3	translate = {};
				DirectX::XMFLOAT4X4	localTransform = {};
				DirectX::XMFLOAT4X4	worldTransform = {};

				std::vector<Node*>	children;
			};

			// �m�[�h����
			Node* FindNode(const char* name);

			void Set();

			// �s��v�Z
			void UpdateTransform(const DirectX::XMFLOAT4X4& transform);
			void UpdateTransform(const WorldContext& wc);
			void UpdateTransform(const WorldContextQ& wc);

			// �O�x�N�g���擾
			const DirectX::XMFLOAT3 GetForward() const { return forward; }

			// �s��擾
			const DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

			// �m�[�h���X�g�擾
			const std::vector<Node>& GetNodes() const { return nodes; }
			std::vector<Node>& GetNodes() { return nodes; }

			// ���\�[�X�擾
			const ModelResource* GetResource() const { return resource.get(); }

			// �A�j���[�V�����X�V����
			void UpdateAnimation(float elapsedTime);

			// �A�j���[�V�����Đ�
			void PlayAnimation(int index, bool loop = false, float blendSeconds = 0.2f);

			// �A�j���[�V�����Đ�����
			bool IsPlayAnimation() const;

			void UpdateSubAnimation(float elapedTime);

			void PlaySubAnimation(const char* subNodeName, int index, bool loop = false, float blendSeconds = 0.2f);

			void StopSubAnimation();

			bool IsPlaySubAnimation() const;

			void SetSubNodeFlag(std::vector<Node*>& nodes, bool flag);

			void ChildrenAnimation(std::vector<Node*>& children, float blendRate, float rate, 
				const ModelResource::Keyframe& keyFrame0, const ModelResource::Keyframe& keyFrame1);

			// �A�j���[�V�����̍Đ����Ԃ�ݒ�(��{0)
			void SetCurrentAnimationSeconds(float currentAnimationSeconds) { this->currentAnimationSeconds = currentAnimationSeconds; }
			// ���݂̃A�j���[�V�����Đ����Ԏ擾
			float GetCurrentAnimationSeconds() const { return currentAnimationSeconds; }

		private:
			std::shared_ptr<ModelResource>	resource;
			std::vector<Node>				nodes;

			DirectX::XMFLOAT3 forward = {};
			DirectX::XMFLOAT4X4	transform = {};

			int currentAnimationIndex = -1;
			float currentAnimationSeconds = 0.0f;

			float animationBlendTime = 0.0f;
			float animationBlendSeconds = 0.0f;

			bool animationLoopFlag = false;
			bool animationEndFlag = false;



			int currentAnimationIndex2 = -1;
			float currentAnimationSeconds2 = 0.0f;

			float animationBlendTime2 = 0.0f;
			float animationBlendSeconds2 = 0.0f;

			bool animationLoopFlag2 = false;
			bool animationEndFlag2 = false;

			Node* subAnimNode = nullptr;

		};



		class LibCopyModel
		{
		public:

			void Load(const char* fileName, std::string id);

			void Release(std::string id);

			void ReleaseAll();

			std::shared_ptr<ModelResource> GetResource(std::string id) { return models.at(id); }

			static LibCopyModel* Instance()
			{
				static LibCopyModel instance;
				return &instance;
			}
		private:

			std::shared_ptr<ModelResource> resource = nullptr;
			std::shared_ptr<LibModel> model = nullptr;

			std::unordered_map<std::string, std::shared_ptr<ModelResource>> models;

		};
	}
}