#pragma once
#include "Asset.h"

namespace LevEngine
{
	class DefaultAsset final : public Asset
	{
	public:
		explicit DefaultAsset(const Path& path, const UUID uuid) : Asset(path, uuid) { }
		
	protected:
		bool WriteDataToFile() const override { return false; }
		bool ReadDataFromFile() const override { return false; }

		void SerializeData(YAML::Emitter& out) override { }
		void DeserializeData(const YAML::Node& node) override { }
	};
}
