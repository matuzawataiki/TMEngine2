#include "Precompiler.h"
#include "TMEngine2.h"
#include "graphics/GraphicsEngine.h"


namespace nsTMEngine2 
{
	TMEngine2* TMEngine2::m_instance = nullptr;

	TMEngine2::TMEngine2()
	{
	}
	TMEngine2::~TMEngine2()
	{
	}

	void TMEngine2::Initialize()
	{
		nsTMEngine2::GraphicsEngine::CreateInstance();
		nsTMEngine2::GraphicsEngine::GetInstance()->Initialize();
	}

	void TMEngine2::Update()
	{
	}

}