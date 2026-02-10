#pragma once
namespace nsTMEngine2
{
	class TMEngine2 : public Noncopyable
	{
	private:
		static TMEngine2* m_instance;
		TMEngine2();
		~TMEngine2();
	public:

		void Initialize();

		void Update();

		static void CreateInstance() { m_instance = new TMEngine2; }
		static void DeleteInstance() { delete m_instance; }
		static TMEngine2* GetInstance() { return m_instance; }

	};

}
