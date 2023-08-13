#pragma once
#include "Asset.h"
#include "Katamari/Selection.h"

namespace LevEngine
{
	class AssetSelection final : public Selection
	{
	public:
		AssetSelection() = default;
		explicit AssetSelection(const Ref<Asset>& asset) : m_Asset(asset) { }

		void Set(const Ref<Asset>& asset) { m_Asset = asset; }
		[[nodiscard]] const Ref<Asset>& Get() const { return m_Asset; }

		void DrawProperties() override
		{
			if (m_Asset)
				m_Asset->DrawProperties();
		}

	private:
		Ref<Asset> m_Asset;
	};
}
