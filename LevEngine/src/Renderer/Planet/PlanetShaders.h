#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	class Shader;

	// The planet's own shaders, kept apart from ShaderAssets for the same reason EnvironmentShaders is:
	// they belong to one subsystem and nothing else has any use for them.
	struct LEV_API PlanetShaders
	{
		//<--- The deferred variant fills the G-buffer; the forward one lights in place ---<<
		static Ref<Shader> Surface();
		static Ref<Shader> SurfaceDeferred();

		//<--- The ocean is always forward: it blends, and a G-buffer cannot hold two surfaces ---<<
		static Ref<Shader> Ocean();
	};
}
