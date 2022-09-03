#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "3D/Renderer3D.h"

namespace LevEngine
{
	void Renderer::Init()
	{
		LEV_PROFILE_FUNCTION();
		
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
	}

	void Renderer::OnWindowResized(const uint32_t width, const uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}
	
	void Renderer::BeginScene()
	{
		
	}

	void Renderer::EndScene()
	{
		
	}

	void Renderer::Submit()
	{
		
	}
}
