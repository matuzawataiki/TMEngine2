#pragma once
namespace nsTMEngine2
{
	class TMEngine2 : public Noncopyable
	{
	private:
		static TMEngine2* instance;
		TMEngine2();
		~TMEngine2();
	public:

		void Initialize();

		void Update();

	};

}
