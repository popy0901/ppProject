#pragma once

#include <memory>
#include <string>
#include <map>
#include "Graphics/ModelResource.h"

namespace PP
{
	namespace Class
	{
		// ���\�[�X�}�l�[�W���[
		class ResourceManager
		{
		private:

			ResourceManager() {}
			~ResourceManager() {}

		public:

			// �C���X�^���X�擾
			static ResourceManager& Instance()
			{
				static ResourceManager instance;
				return instance;
			}

			// ���f�����\�[�X�ǂݍ���
			std::shared_ptr<ModelResource> LoadModelResource(const char* fileName);

		private:

			std::map<std::string, std::weak_ptr<ModelResource>> models;

		};
	}
}