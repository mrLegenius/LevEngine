#pragma once
#include "Asset.h"

namespace LevEngine
{
	class DefaultAsset final : public Asset
	{
	public:
		explicit DefaultAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }

		void DrawProperties() override { }

	protected:
		bool OverrideDataFile() const override { return false; }

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(YAML::Node& node) override { }
	};
}
