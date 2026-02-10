#pragma once
namespace nsTMEngine2
{
	class GameObjectManager : public Noncopyable
	{
	private:
		static GameObjectManager* m_instance;
		GameObjectManager();
		~GameObjectManager();

	private:
		bool m_isActive = false;
		enum { GAME_OBJECT_PRIO_MAX = 255 };
		std::array<std::list<IGameObject*>, GAME_OBJECT_PRIO_MAX> m_gameObjectList;

	public:

		template<class T>
		T* NewGameObject(int prio,const char* objectName){
			if (!m_isActive) {
				return nullptr;
			}
			T* newObject = new T();
			newObject->SetName(objectName);
			m_gameObjectList.at(prio).push_back(newObject);
			return newObject;
		}

		void DeleteGameObject(IGameObject* gameObject) {
			if (!m_isActive) {
				return;
			}
			if (gameObject != nullptr) {
				gameObject->Dead();
			}
		}

		template<class T>
		const T* FindGameObject(const char* objectName) {
			if (!m_isActive) {
				return nullptr;
			}

			for (auto FindList : m_gameObjectList) {
				for (auto gameObject : FindList) {
					if (gameObject->IsMatcName(objectName)) {
						T* p = dynamic_cast<T*>(gameObject);
						return p;
					}
				}
			}
			return nullptr;
		}

		template<class T>
		const std::vector<T*>& FindGameObjects(const char* objectName)
		{
			static std::vector<T*> objectVector;
			objectVector.clear();
			if (m_isActive == false)
			{
				return objectVector;
			}
			for (auto findList : m_gameObjectListArray) {
				for (auto gameObject : findList) {
					if (gameObject->IsMatchName(objectName)) {
						T* p = dynamic_cast<T*>(gameObject);
						objectVector.push_back(p);
					}
				}
			}
			return objectVector;
		}

		void ExecuteUpdate();

		void ExecuteRender();


		static void CreateInstance() { m_instance = new GameObjectManager; }
		static void DeleteInstance() { delete m_instance; }
		static GameObjectManager* GetInstance() { return m_instance; }
	};

	template<class T>
	static inline const T* NewGO(int prio, const char* objectName) {
		return GameObjectManager::GetInstance()->NewGameObject<T>(prio, objectName);
	}

	static inline void DeleteGO(IGameObject* gameObject) {
		GameObjectManager::GetInstance()->DeleteGameObject(gameObject);
	}

	template<class T>
	static inline const T* FindGO(const char* objectName) {
		return GameObjectManager::GetInstance()->FindGameObject(objectName);
	}

	template<class T>
	static inline const std::vector<T*>& FindGOs(const char* objectName) {
		return GameObjectManager::GetInstance()->FindGameObjects(objectName);
	}

}
