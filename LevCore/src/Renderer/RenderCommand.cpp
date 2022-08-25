#include "RenderCommand.h"

#include "Platforms/OpenGL/OpenGLRendererAPI.h"

namespace LevEngine
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}
