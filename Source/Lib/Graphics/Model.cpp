#include "Lib/Graphics/Graphics.h"
#include "Lib/Graphics/Model.h"
#include "Lib/ResourceManager.h"

namespace PP
{
	namespace Class
	{
		// コンストラクタ
		LibModel::LibModel(std::string id)
		{
			// リソース読み込み
			//resource = std::make_shared<ModelResource>();
			//resource->Load(Graphics::Instance().GetDevice(), filename);

			//resource = ResourceManager::Instance().LoadModelResource(filename);

			resource = LibCopyModel::Instance()->GetResource(id);

			Set();
		}

		LibModel::Node* LibModel::FindNode(const char* name)
		{
			// 全てのノードを総当たりで名前比較
			for (Node& node : nodes)
			{
				if (::strcmp(node.name, name) == 0)
				{
					return &node;
				}
			}

			// 見つからなかった
			return nullptr;
		}

		void LibModel::Set()
		{
			// ノード
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

			// 行列計算
			const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			UpdateTransform(transform);
		}

		// 変換行列計算
		void LibModel::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
		{
			forward = { transform._31, transform._32 , transform._33 };

			DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

			for (Node& node : nodes)
			{
				// ローカル行列算出
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
				DirectX::XMMATRIX LocalTransform = S * R * T;

				// ワールド行列算出
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

				// 計算結果を格納
				DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
				DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
			}
		}

		// 変換行列計算
		void LibModel::UpdateTransform(const WorldContext& wc)
		{
			// ワールド行列計算
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(wc.scale.x, wc.scale.y, wc.scale.z);

			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(wc.angle.x, wc.angle.y, wc.angle.z);

			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(wc.position.x, wc.position.y, wc.position.z);

			DirectX::XMMATRIX Transform = S * R * T;

			DirectX::XMStoreFloat4x4(&transform, Transform);
			forward = { transform._31, transform._32 , transform._33 };

			for (Node& node : nodes)
			{
				// ローカル行列算出
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
				DirectX::XMMATRIX LocalTransform = S * R * T;

				// ワールド行列算出
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

				// 計算結果を格納
				DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
				DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
			}
		}

		// 変換行列計算
		void LibModel::UpdateTransform(const WorldContextQ& wc)
		{
			// ワールド行列計算
			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(wc.scale.x, wc.scale.y, wc.scale.z);

			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion
			(DirectX::XMVectorSet(wc.quaternion.x, wc.quaternion.y, wc.quaternion.z, wc.quaternion.w));

			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(wc.position.x, wc.position.y, wc.position.z);

			DirectX::XMMATRIX Transform = S * R * T;

			for (Node& node : nodes)
			{
				// ローカル行列算出
				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
				DirectX::XMMATRIX LocalTransform = S * R * T;

				// ワールド行列算出
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

				// 計算結果を格納
				DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
				DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
			}
		}

		void LibModel::UpdateAnimation(float elapsedTime)
		{
			// 再生中でないなら処理はしない
			if (!IsPlayAnimation()) return;

			// ブレンド率の計算
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

			// 指定のアニメーションデータを取得
			const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
			const ModelResource::Animation& animation = animations.at(currentAnimationIndex);

			// アニメーションデータからキーフレームデータリストを取得
			const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				int keyIndex1 = keyIndex + 1;
				const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex1);
				if (currentAnimationSeconds >= keyframe0.seconds &&
					currentAnimationSeconds < keyframe1.seconds)
				{
					// 再生時間とキーフレームの時間から補完率を算出する
					float rate = (currentAnimationSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

					int nodeCount = static_cast<int>(nodes.size());
					for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
					{
						// ２つのキーフレーム間の補完計算
						const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
						const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

						Node& node = nodes[nodeIndex];

						if (node.flag)
							continue;
						// ブレンド補完処理
						if (blendRate < 1.0f)
						{
							// 現在の姿勢と次のフレームとの姿勢の補完
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
							node.translate.z = 0; // モーションによるモデル座標系でのずれをなくす
						}
						// 通常の計算
						else
						{
							// 前のキーフレームと次のキーフレームの姿勢を補完
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
							node.translate.z = 0; // モーションによるモデル座標系でのずれをなくす
						}
					}
					break;
				}
			}

			// 最終フレーム処理
			if (animationEndFlag)
			{
				animationEndFlag = false;
				currentAnimationIndex = -1;
				return;
			}

			// 時間経過
			currentAnimationSeconds += elapsedTime;

			// 再生時間が終端時間を越えたら
			if (currentAnimationSeconds >= animation.secondsLength)
			{
				// 再生時間を巻き戻す
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
			// 再生中でないなら処理はしない
			if (!IsPlayAnimation()) return;

			// ノードを指定していないなら処理はしない
			if (subAnimNode == nullptr) return;

			// ブレンド率の計算
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

			// 指定のアニメーションデータを取得
			const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
			const ModelResource::Animation& animation = animations.at(currentAnimationIndex2);

			// アニメーションデータからキーフレームデータリストを取得
			const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
			int keyCount = static_cast<int>(keyframes.size());
			for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
			{
				// 現在の時間がどのキーフレームの間にいるか判定する
				int keyIndex1 = keyIndex + 1;
				const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex1);
				if (currentAnimationSeconds2 >= keyframe0.seconds &&
					currentAnimationSeconds2 < keyframe1.seconds)
				{
					// 再生時間とキーフレームの時間から補完率を算出する
					float rate = (currentAnimationSeconds2 - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

					int nodeCount = static_cast<int>(nodes.size());
					for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
					{
						// ２つのキーフレーム間の補完計算
						const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
						const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

						Node& node = nodes[nodeIndex];
						if (!node.flag) 
							continue;

						if (node.name == subAnimNode->name)
						{
							// ブレンド補完処理
							if (blendRate < 1.0f)
							{
								// 現在の姿勢と次のフレームとの姿勢の補完
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
							// 通常の計算
							else
							{
								// 前のキーフレームと次のキーフレームの姿勢を補完
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

			// 最終フレーム処理
			if (animationEndFlag2)
			{
				animationEndFlag2 = false;
				currentAnimationIndex2 = -1;
				return;
			}

			// 時間経過
			currentAnimationSeconds2 += elapsedTime;

			// 再生時間が終端時間を越えたら
			if (currentAnimationSeconds2 >= animation.secondsLength)
			{
				// 再生時間を巻き戻す
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
			const ModelResource::Keyframe& keyFrame0, const ModelResource::Keyframe& keyFrame1) // 再起関数
		{
			for (int i = 0; i < children.size(); i++)
			{
				const ModelResource::NodeKeyData& key0 = keyFrame0.nodeKeys.at(children.at(i)->Index);
				const ModelResource::NodeKeyData& key1 = keyFrame1.nodeKeys.at(children.at(i)->Index);

				// ブレンド補完処理
				if (blendRate < 1.0f)
				{
					// 現在の姿勢と次のフレームとの姿勢の補完
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

					//children.at(i)->translate.z = 0; // モーションによるモデル座標系でのずれをなくす
				}
				// 通常の計算
				else
				{
					// 前のキーフレームと次のキーフレームの姿勢を補完
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

					//children.at(i)->translate.z = 0; // モーションによるモデル座標系でのずれをなくす
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