#include "pch.h"
#include "RenderCommand.h"
namespace LevEngine
{
//TODO: Create from RenderAPI enum from settings
D3D11RendererAPI* RenderCommand::s_RendererAPI = new D3D11RendererAPI;
}