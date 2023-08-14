#pragma once
#include "yaml-cpp/emitter.h"

namespace LevEngine
{
	class Asset
	{
	public:
		explicit Asset(std::filesystem::path path)
			: m_Name(path.stem().string())
			, m_FullName(path.filename().string())
			, m_Extension(path.extension().string())
			, m_Path(std::move(path)) { }

		virtual ~Asset() = default;

		const std::string& GetName() const { return m_Name; }
		const std::string& GetFullName() const { return m_FullName; }
		const std::string& GetExtension() const { return m_Extension; }
		const std::filesystem::path& GetPath() const { return m_Path; }

		void Serialize();
		bool Deserialize();

		virtual void DrawProperties() = 0;

	protected:
		virtual void SerializeData(YAML::Emitter& out) = 0;
		virtual bool DeserializeData(YAML::Node& node) = 0;
		std::string m_Name;
		std::string m_FullName;
		std::string m_Extension;
		std::filesystem::path m_Path;
	};
}
