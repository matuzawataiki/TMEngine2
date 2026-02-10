#include "Precompiler.h"
#include "GameObjectManager.h"
namespace nsTMEngine2
{
	GameObjectManager* GameObjectManager::m_instance = nullptr;

	GameObjectManager::GameObjectManager()
	{
	}
	GameObjectManager::~GameObjectManager()
	{
	}

	void GameObjectManager::ExecuteUpdate()
	{
		for (auto& deleteList : m_gameObjectList) {
			deleteList.remove_if([&](IGameObject* object) {
				if (object->IsDead()) {
					delete object;
					return true;
				}
				return false;
			});
		}

		for (auto& objectList : m_gameObjectList) {
			for (auto& object : objectList) {
				object->StartWrapper();
			}
		}

		for (auto& objectList : m_gameObjectList) {
			for (auto& object : objectList) {
				object->UpdateWapper();
			}
		}
	}

	void GameObjectManager::ExecuteRender()
	{
	}


}
