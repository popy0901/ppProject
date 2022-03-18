#pragma once

#include <memory>
#include <string>
#include <map>
#include "Graphics/ModelResource.h"

namespace PP
{
	namespace Class
	{
		// リソースマネージャー
		class ResourceManager
		{
		private:

			ResourceManager() {}
			~ResourceManager() {}

		public:

			// インスタンス取得
			static ResourceManager& Instance()
			{
				static ResourceManager instance;
				return instance;
			}

			// モデルリソース読み込み
			std::shared_ptr<ModelResource> LoadModelResource(const char* fileName);

		private:

			std::map<std::string, std::weak_ptr<ModelResource>> models;

		};
	}
}