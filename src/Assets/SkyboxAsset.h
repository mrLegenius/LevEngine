#pragma once
#include "Asset.h"
#include "GUI/GUIUtils.h"

namespace LevEngine
{
	class SkyboxAsset final : public Asset
	{
	public:
		enum class Side
		{
			Left,
			Right,
			Bottom,
			Top,
			Back,
			Front
		};

		explicit SkyboxAsset(const std::filesystem::path& path)
			: Asset(path)
		{

		}

		void DrawProperties() override;

		[[nodiscard]] const Ref<Texture>& GetTexture() const { return m_Texture; }

		void SetTexture(Side side, std::string path);

	protected:
		void Serialize(YAML::Emitter& out) override;
		bool Deserialize(YAML::Node& node) override;
	private:
		void InitTexture();

		Ref<Texture> m_Texture;

		std::string m_Left;
		std::string m_Right;
		std::string m_Bottom;
		std::string m_Top;
		std::string m_Back;
		std::string m_Front;
	};
}

