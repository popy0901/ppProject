#include "Lib/Graphics/Graphics.h"
#include "Lib/Graphics/Model.h"
#include "Lib/ResourceManager.h"

namespace PP
{
	namespace Class
	{
		// �R���X�g���N�^
		LibModel::LibModel(std::string id)
		{
			// ���\�[�X�ǂݍ���
			//resource = std::make_shared<ModelResource>();
			//resource->Load(Graphics::Instance().GetDevice(), filename);

			//resource = ResourceManager::Instance().LoadModelResource(filename);

			resource = LibCopyModel::Instance()->GetResource(id);

			Set();
		}

		LibModel::Node* LibModel::FindNode(const char* name)
		{
			// �S�Ẵm�[�h�𑍓�����Ŗ��O��r
			for (Node& node : nodes)
			{
				if (::strcmp(node.name, name) == 0)
				{
					return &node;
				}
			}

			// ������Ȃ�����
			return nullptr;
		}

		void LibModel::Set()
		{
			// �m�[�h
			const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

			nodes.resize(resNodes.size());
			for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
			{
				auto&& src = resNodes.at(nodeIndex);
				auto&& dst = nodes.at(nodeIndex);
				dst.Index = nodeIndex;
				dst.name = src.name.c_str();
				dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
				dst.scale = src.scale;
				dst.rotate = src.rotate;
				dst.translate = src.translate;

				if (dst.parent != nullptr)
				{
					dst.parent->children.emplace_back(&dst);
				}
			}

			// �s��v�Z
			const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			UpdateTransform(transform);
		}

		// �ϊ��s��v�Z
		void LibModel::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
		{
			forward = { transform._31, transform._32 , transform._33 };

			DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

			for (Node& node : nodes)
			{
				// ���[�J���s��Z�o
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
				DirectX::XMMATRIX LocalTransform = S * R * T;

				// ���[���h�s��Z�o
				DirectX::XMMATRIX ParentTransform;
				if (node.parent != nullptr)
				{
					ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
				}
				else
				{
					ParentTransform = Transform;
				}
				DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

				// �v�Z���ʂ��i�[
				DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
				DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
			}
		}

		// �ϊ��s��v�Z
		void LibModel::UpdateTransform(const WorldContext& wc)
		{
			// ���[���h�s��v�Z
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(wc.scale.x, wc.scale.y, wc.scale.z);

			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(wc.angle.x, wc.angle.y, wc.angle.z);

			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(wc.position.x, wc.position.y, wc.position.z);

			DirectX::XMMATRIX Transform = S * R * T;

			DirectX::XMStoreFloat4x4(&transform, Transform);
			forward = { transform._31, transform._32 , transform._33 };

			for (Node& node : nodes)
			{
				// ���[�J���s��Z�o
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
				DirectX::XMMATRIX LocalTransform = S * R * T;

				// ���[���h�s��Z�o
				DirectX::XMMATRIX ParentTransform;
				if (node.parent != nullptr)
				{
					ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
				}
				else
				{
					ParentTransform = Transform;
				}
				DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

				// �v�Z���ʂ��i�[
				DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
				DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
			}
		}

		// �ϊ��s��v�Z
		void LibModel::UpdateTransform(const WorldContextQ& wc)
		{
			// ���[���h�s��v�Z
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(wc.scale.x, wc.scale.y, wc.scale.z);

			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion
			(DirectX::XMVectorSet(wc.quaternion.x, wc.quaternion.y, wc.quaternion.z, wc.quaternion.w));

			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(wc.position.x, wc.position.y, wc.position.z);

			DirectX::XMMATRIX Transform = S * R * T;

			for (Node& node : nodes)
			{
				// ���[�J���s��Z�o
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
				DirectX::XMMATRIX LocalTransform = S * R * T;

				// ���[���h�s��Z�o
				DirectX::XMMATRIX ParentTransform;
				if (node.parent != nullptr)
				{
					ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
				}
				else
				{
					ParentTransform = Transform;
				}
				DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

				// �v�Z���ʂ��i�[
				DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
				DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
			}
		}

		void LibModel::UpdateAnimation(float elapsedTime)
		{
			// �Đ����łȂ��Ȃ珈���͂��Ȃ�
			if (!IsPlayAnimation()) return;

			// �u�����h���̌v�Z
			float blendRate = 1.0f;
			if (animationBlendTime <= animationBlendSeconds)
			{
				animationBlendTime += elapsedTime;
				if (animationBlendTime <= animationBlendSeconds)
				{
					animationBlendTime -= animationBlendSeconds;
				}
				blendRate = animationBlendTime - animationBlendSeconds;
				blendRate *= blendRate;
			}

			// �w��̃A�j���[�V�����f�[�^���擾
			const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
			const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

			// �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
			const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				int keyIndex1 = keyIndex + 1;
				const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex1);
				if (currentAnimationSeconds >= keyframe0.seconds &&
					currentAnimationSeconds < keyframe1.seconds)
				{
					// �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
					float rate = (currentAnimationSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

					int nodeCount = static_cast<int>(nodes.size());
					for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
					{
						// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
						const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
						const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

						Node& node = nodes[nodeIndex];

						if (node.flag)
							continue;
						// �u�����h�⊮����
						if (blendRate < 1.0f)
						{
							// ���݂̎p���Ǝ��̃t���[���Ƃ̎p���̕⊮
							DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&node.scale);
							DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
							DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&node.rotate);
							DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
							DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&node.translate);
							DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

							DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, blendRate);
							DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, blendRate);
							DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, blendRate);

							DirectX::XMStoreFloat3(&node.scale, s);
							DirectX::XMStoreFloat4(&node.rotate, r);
							DirectX::XMStoreFloat3(&node.translate, t);
							//node.translate.x = 0;
							node.translate.z = 0; // ���[�V�����ɂ�郂�f�����W�n�ł̂�����Ȃ���
						}
						// �ʏ�̌v�Z
						else
						{
							// �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
							DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
							DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
							DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
							DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
							DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
							DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

							DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
							DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
							DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

							DirectX::XMStoreFloat3(&node.scale, s);
							DirectX::XMStoreFloat4(&node.rotate, r);
							DirectX::XMStoreFloat3(&node.translate, t);
							//node.translate.x = 0;
							node.translate.z = 0; // ���[�V�����ɂ�郂�f�����W�n�ł̂�����Ȃ���
						}
					}
					break;
				}
			}

			// �ŏI�t���[������
			if (animationEndFlag)
			{
				animationEndFlag = false;
				currentAnimationIndex = -1;
				return;
			}

			// ���Ԍo��
			currentAnimationSeconds += elapsedTime;

			// �Đ����Ԃ��I�[���Ԃ��z������
			if (currentAnimationSeconds >= animation.secondsLength)
			{
				// �Đ����Ԃ������߂�
				//currentAnimationSeconds -= animation.secondsLength;
				if (animationLoopFlag)
				{
					currentAnimationSeconds -= animation.secondsLength;
				}
				else
				{
					currentAnimationSeconds = animation.secondsLength;
					animationEndFlag = true;
				}
			}
		}

		void LibModel::PlayAnimation(int index, bool loop, float blendSeconds)
		{
			currentAnimationIndex = index;
			SetCurrentAnimationSeconds(0.0f);
			animationLoopFlag = loop;
			animationEndFlag = false;
			animationBlendTime = 0.0f;
			animationBlendSeconds = blendSeconds;
		}

		bool LibModel::IsPlayAnimation() const
		{
			if (currentAnimationIndex < 0) return false;
			if (currentAnimationIndex >= resource->GetAnimations().size()) return false;
			return true;
		}

		void LibModel::UpdateSubAnimation(float elapsedTime)
		{
			// �Đ����łȂ��Ȃ珈���͂��Ȃ�
			if (!IsPlayAnimation()) return;

			// �m�[�h���w�肵�Ă��Ȃ��Ȃ珈���͂��Ȃ�
			if (subAnimNode == nullptr) return;

			// �u�����h���̌v�Z
			float blendRate = 1.0f;
			if (animationBlendTime2 <= animationBlendSeconds2)
			{
				animationBlendTime2 += elapsedTime;
				if (animationBlendTime2 <= animationBlendSeconds2)
				{
					animationBlendTime2 -= animationBlendSeconds2;
				}
				blendRate = animationBlendTime2 - animationBlendSeconds2;
				blendRate *= blendRate;
			}

			// �w��̃A�j���[�V�����f�[�^���擾
			const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
			const ModelResource::Animation& animation = animations.at(currentAnimationIndex2);

			// �A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
			const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				int keyIndex1 = keyIndex + 1;
				const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex1);
				if (currentAnimationSeconds2 >= keyframe0.seconds &&
					currentAnimationSeconds2 < keyframe1.seconds)
				{
					// �Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
					float rate = (currentAnimationSeconds2 - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

					int nodeCount = static_cast<int>(nodes.size());
					for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
					{
						// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
						const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
						const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

						Node& node = nodes[nodeIndex];
						if (!node.flag) 
							continue;

						if (node.name == subAnimNode->name)
						{
							// �u�����h�⊮����
							if (blendRate < 1.0f)
							{
								// ���݂̎p���Ǝ��̃t���[���Ƃ̎p���̕⊮
								DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&node.scale);
								DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
								DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&node.rotate);
								DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
								DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&node.translate);
								DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

								DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, blendRate);
								DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, blendRate);
								DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, blendRate);

								DirectX::XMStoreFloat3(&node.scale, s);
								DirectX::XMStoreFloat4(&node.rotate, r);
								DirectX::XMStoreFloat3(&node.translate, t);
							}
							// �ʏ�̌v�Z
							else
							{
								// �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
								DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
								DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
								DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
								DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
								DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
								DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

								DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
								DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
								DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

								DirectX::XMStoreFloat3(&node.scale, s);
								DirectX::XMStoreFloat4(&node.rotate, r);
								DirectX::XMStoreFloat3(&node.translate, t);
							}

							ChildrenAnimation(node.children, blendRate, rate, keyframe0, keyframe1);
						}

					}
					break;
				}
			}

			// �ŏI�t���[������
			if (animationEndFlag2)
			{
				animationEndFlag2 = false;
				currentAnimationIndex2 = -1;
				return;
			}

			// ���Ԍo��
			currentAnimationSeconds2 += elapsedTime;

			// �Đ����Ԃ��I�[���Ԃ��z������
			if (currentAnimationSeconds2 >= animation.secondsLength)
			{
				// �Đ����Ԃ������߂�
				//currentAnimationSeconds2 -= animation.secondsLength;
				if (animationLoopFlag2)
				{
					currentAnimationSeconds2 -= animation.secondsLength;
				}
				else
				{
					currentAnimationSeconds2 = animation.secondsLength;
					animationEndFlag2 = true;
				}
			}
		}

		void LibModel::PlaySubAnimation(const char* subNodeName, int index, bool loop, float blendSeconds)
		{
			subAnimNode = FindNode(subNodeName);

			subAnimNode->flag = true;

			if (subAnimNode->children.size() > 0)
				SetSubNodeFlag(subAnimNode->children, true);

			currentAnimationIndex2 = index;
			currentAnimationSeconds2 = 0.0f;
			animationLoopFlag2 = loop;
			animationEndFlag2 = false;
			animationBlendTime2 = 0.0f;
			animationBlendSeconds2 = blendSeconds;
		}

		void LibModel::StopSubAnimation()
		{
			if (subAnimNode == nullptr) return;
			if (subAnimNode->children.size() > 0)
				SetSubNodeFlag(subAnimNode->children, false);

			subAnimNode->flag = false;

			subAnimNode = nullptr;
		}

		bool LibModel::IsPlaySubAnimation() const
		{
			if (currentAnimationIndex2 < 0) return false;
			if (currentAnimationIndex2 >= resource->GetAnimations().size()) return false;
			return true;
		}

		void LibModel::SetSubNodeFlag(std::vector<Node*>& nodes, bool flag)
		{
			for (auto& it : nodes)
			{
				it->flag = flag;
				if (it->children.size() > 0)
				{
					SetSubNodeFlag(it->children, flag);
				}
			}
		}

		void LibModel::ChildrenAnimation(std::vector<Node*>& children, float blendRate, float rate,
			const ModelResource::Keyframe& keyFrame0, const ModelResource::Keyframe& keyFrame1) // �ċN�֐�
		{
			for (int i = 0; i < children.size(); i++)
			{
				const ModelResource::NodeKeyData& key0 = keyFrame0.nodeKeys.at(children.at(i)->Index);
				const ModelResource::NodeKeyData& key1 = keyFrame1.nodeKeys.at(children.at(i)->Index);

				// �u�����h�⊮����
				if (blendRate < 1.0f)
				{
					// ���݂̎p���Ǝ��̃t���[���Ƃ̎p���̕⊮
					DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&children.at(i)->scale);
					DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&children.at(i)->rotate);
					DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&children.at(i)->translate);
					DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, blendRate);
					DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, blendRate);
					DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, blendRate);

					DirectX::XMStoreFloat3(&children.at(i)->scale, s);
					DirectX::XMStoreFloat4(&children.at(i)->rotate, r);
					DirectX::XMStoreFloat3(&children.at(i)->translate, t);

					//children.at(i)->translate.z = 0; // ���[�V�����ɂ�郂�f�����W�n�ł̂�����Ȃ���
				}
				// �ʏ�̌v�Z
				else
				{
					// �O�̃L�[�t���[���Ǝ��̃L�[�t���[���̎p����⊮
					DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
					DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
					DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
					DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
					DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
					DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

					DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
					DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
					DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

					DirectX::XMStoreFloat3(&children.at(i)->scale, s);
					DirectX::XMStoreFloat4(&children.at(i)->rotate, r);
					DirectX::XMStoreFloat3(&children.at(i)->translate, t);

					//children.at(i)->translate.z = 0; // ���[�V�����ɂ�郂�f�����W�n�ł̂�����Ȃ���
				}
			}

			for (int j = 0; j < children.size(); j++)
			{
				if (children.at(j)->children.size())
					ChildrenAnimation(children.at(j)->children, blendRate, rate, keyFrame0, keyFrame1);
			}

		}



		void LibCopyModel::Load(const char* fileName, std::string id)
		{
			resource = ResourceManager::Instance().LoadModelResource(fileName);

			models.insert(std::make_pair(id, resource));
		}

		void LibCopyModel::Release(std::string id)
		{
			models.erase(models.find(id));
		}

		void LibCopyModel::ReleaseAll()
		{
			if (!models.size()) return;

			std::unordered_map<std::string, std::shared_ptr<ModelResource>>
				::iterator it = models.begin();
			while (it != models.end())
			{
				models.erase(it++);
			}
		}
	}
}