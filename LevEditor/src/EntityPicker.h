#pragma once

namespace LevEngine
{
	class Scene;
}

namespace LevEngine::Editor
{
	class EntityPicker
	{
	public:
		//<--- Returns the entity whose geometry the ray hits first, only entities with a mesh can be picked ---<<
		static Entity Pick(const Ref<Scene>& scene, const Ray& ray);
	};
}
