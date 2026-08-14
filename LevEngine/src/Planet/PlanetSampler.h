#pragma once

#include "Kernel/Core.h"
#include "PlanetShape.h"
#include "PlanetClimate.h"

namespace LevEngine
{
	// Everything the generator knows about one point on the surface.
	struct PlanetSurfaceSample
	{
		PlanetShapeSample Shape;
		PlanetClimateSample Climate;
	};

	// Shape and climate in one object, because nothing wants one without the other and the climate
	// needs the shape's intermediate values. This is the handle the mesh builder, the collision
	// cooker, gameplay queries and the editor preview all hold -- one copy per planet, read from
	// several threads at once, which is why it has no mutable state and nothing to lock.
	class LEV_API PlanetSampler
	{
	public:
		PlanetSampler() = default;

		PlanetSampler(const PlanetShapeSettings& shape, const PlanetClimateSettings& climate)
			: m_Shape(shape), m_Climate(climate) { }

		[[nodiscard]] const PlanetShape& GetShape() const { return m_Shape; }
		[[nodiscard]] const PlanetClimate& GetClimate() const { return m_Climate; }

		[[nodiscard]] float GetRadius() const { return m_Shape.GetSettings().Radius; }

		[[nodiscard]] PlanetSurfaceSample Sample(const Vector3 direction) const
		{
			PlanetSurfaceSample sample;
			sample.Shape = m_Shape.Sample(direction);
			sample.Climate = m_Climate.Sample(m_Shape, direction, sample.Shape);

			return sample;
		}

		//<--- Shape only, for callers that do not care what grows there -- collision, and the
		//rain shadow's own upwind lookup ---<<
		[[nodiscard]] float GetElevation(const Vector3 direction) const { return m_Shape.GetElevation(direction); }

		[[nodiscard]] float GetSurfaceRadius(const Vector3 direction) const
		{
			return m_Shape.GetSurfaceRadius(direction);
		}

		[[nodiscard]] Vector3 GetSurfacePoint(const Vector3 direction) const
		{
			return m_Shape.GetSurfacePoint(direction);
		}

	private:
		PlanetShape m_Shape;
		PlanetClimate m_Climate;
	};
}
