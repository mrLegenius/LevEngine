#include "pch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer3D.h"
namespace LevEngine
{
void Renderer::Init()
{
	RenderCommand::Init();
	Renderer3D::Init();
}

void Renderer::Render()
{
	//Renderer2D::Render();
}

void Renderer::Shutdown()
{
	Renderer3D::Shutdown();
}
}