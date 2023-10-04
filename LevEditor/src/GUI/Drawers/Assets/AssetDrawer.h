#pragma once

namespace LevEngine::Editor
{
	class IAssetDrawer
	{
	public:
		virtual void Draw(Ref<Asset> assetRef) = 0;
	};

	template<typename TAsset, class TDrawer, int Order = 1000>
	class AssetDrawer : public IAssetDrawer
	{
	public:
		virtual ~AssetDrawer() = default;

		void Draw(Ref<Asset> assetRef) override
		{
			// Check that used drawer type is meant for the received assetRef.  
			auto castAssetRef = eastl::dynamic_pointer_cast<TAsset>(assetRef);
			if (castAssetRef == nullptr)
			{
				return;	
			}

			ImGui::BeginChild("asset_properties");
			DrawContent(castAssetRef);
			ImGui::EndChild();
		}

	protected:
		[[nodiscard]] virtual String GetLabel() const = 0;
		[[nodiscard]] virtual bool IsRemovable() const { return true; }

		virtual void DrawContent(Ref<TAsset> assetRef) = 0;

	private:
		static inline OrderedClassRegister<IAssetDrawer, TDrawer, Order> s_ClassRegister;
	};
}