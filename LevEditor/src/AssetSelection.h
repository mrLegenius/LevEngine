#pragma once
#include "Selection.h"

namespace LevEngine
{
	class Asset;
	class AssetSelection final : public Selection
	{
	public:
		AssetSelection() = default;
		explicit AssetSelection(const Ref<Asset>& asset) : m_Asset(asset) { }

		void Set(const Ref<Asset>& asset) { m_Asset = asset; }
		[[nodiscard]] const Ref<Asset>& Get() const { return m_Asset; }

		void DrawProperties() override;

	private:
		Ref<Asset> m_Asset;
	};
}
