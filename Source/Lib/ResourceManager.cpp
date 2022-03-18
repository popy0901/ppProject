#include "Graphics/Graphics.h"
#include "ResourceManager.h"


namespace PP
{
	namespace Class
	{
		std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* fileName)
		{
			// モデルを検索
			std::map<std::string, std::weak_ptr<ModelResource>>::iterator it = models.find(fileName);
			if (it != models.end())
			{
				// 寿命が切れているか確認
				if (!it->second.expired()) // 切れていない(リンクしている)
				{
					// 読み込み済みのリソースを返す
					return  it->second.lock(); // 見ているshared_ptrを取得
				}
			}

			// 新規モデルリソース作成＆読み込み
			auto model = std::make_shared<ModelResource>();
			model->Load(Graphics::Instance().GetDevice(), fileName);

			// マップに登録
			models.insert(std::make_pair(fileName, model));

			return model;
		}
	}
}