#pragma once
#include <yaml-cpp/yaml.h>

namespace LevEngine
{
	class Asset
	{
	public:
		explicit Asset(std::filesystem::path path, const UUID uuid)
			: m_Name(path.stem().string())
			, m_FullName(path.filename().string())
			, m_Extension(path.extension().string())
			, m_MetaPath(path.string().append(".meta"))
			, m_Path(std::move(path))
			, m_UUID(uuid)
		{
		}

		virtual ~Asset() = default;

		[[nodiscard]] const std::string& GetName() const { return m_Name; }
		[[nodiscard]] const std::string& GetFullName() const { return m_FullName; }
		[[nodiscard]] const std::string& GetExtension() const { return m_Extension; }
		[[nodiscard]] const std::filesystem::path& GetPath() const { return m_Path; }
		[[nodiscard]] const UUID& GetUUID() const { return m_UUID; }
		[[nodiscard]] bool IsDeserialized() const { return m_Deserialized; }

		void Serialize();
		bool Deserialize();
		void SerializeMeta();

		void DrawEditor();

	protected:
		virtual void DrawProperties() { }

		[[nodiscard]] virtual bool DoSerializeData() const { return true; }
		virtual void SerializeData(YAML::Emitter& out) = 0;
		virtual void DeserializeData(YAML::Node& node) = 0;

		virtual void SerializeMeta(YAML::Emitter& out) { }
		virtual void DeserializeMeta(YAML::Node& node) { }

		std::string m_Name;
		std::string m_FullName;
		std::string m_Extension;
		std::filesystem::path m_MetaPath;
		std::filesystem::path m_Path;
		UUID m_UUID;
		bool m_Deserialized = false;

	private:
		void SerializeData();
		bool DeserializeData();
		bool DeserializeMeta();
	};
}
