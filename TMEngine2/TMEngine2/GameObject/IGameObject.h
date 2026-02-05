#pragma once
namespace nsTMEngine2
{
	class IGameObject : public Noncopyable
	{
	private:
		std::string m_name = "Nameless";
		bool m_isStart = false;
		bool m_isDead = false;
		bool m_isRegistDeadList = false;
		bool m_isNewFromGameObjectManager;
		bool m_isRegist = false;
		bool m_isActive = true;	

	public:
		virtual ~IGameObject(){}

		virtual bool Start() { return true; };

		virtual void Update(){}

		bool IsStart() const {return m_isStart;}

		bool IsDead() const {return m_isDead;}

		bool IsActive() const {return m_isActive;}

		void Activate() { m_isActive = true; }

		void Deactivate() { m_isActive = true; }

		void Dead() { m_isDead = true; }

		void SetName(const char* name){
			if (name != nullptr) {
				m_name = name;
			}
		}

		bool IsMatcName(const char* name) const {
			if (name == m_name) {
				return true;
			}
			return false;
		}

	};

}
