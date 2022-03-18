#include "Graphics/Graphics.h"
#include "ResourceManager.h"


namespace PP
{
	namespace Class
	{
		std::shared_ptr<ModelResource> ResourceManager::LoadModelResource(const char* fileName)
		{
			// ���f��������
			std::map<std::string, std::weak_ptr<ModelResource>>::iterator it = models.find(fileName);
			if (it != models.end())
			{
				// �������؂�Ă��邩�m�F
				if (!it->second.expired()) // �؂�Ă��Ȃ�(�����N���Ă���)
				{
					// �ǂݍ��ݍς݂̃��\�[�X��Ԃ�
					return  it->second.lock(); // ���Ă���shared_ptr���擾
				}
			}

			// �V�K���f�����\�[�X�쐬���ǂݍ���
			auto model = std::make_shared<ModelResource>();
			model->Load(Graphics::Instance().GetDevice(), fileName);

			// �}�b�v�ɓo�^
			models.insert(std::make_pair(fileName, model));

			return model;
		}
	}
}