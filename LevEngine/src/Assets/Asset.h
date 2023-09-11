#pragma once
#include <yaml-cpp/yaml.h>

#include "DataTypes/Path.h"
#include "DataTypes/String.h"

namespace LevEngine
{
	class Asset
	{
	public:
		explicit Asset(const Path& path, const UUID uuid)
			: m_Name(path.stem().string().c_str())
			, m_FullName(path.filename().string().c_str())
			, m_Extension(path.extension().string().c_str())
			, m_MetaPath(path.string().append(".meta").c_str())
			, m_Path(path)
			, m_UUID(uuid)
		{
		}

		virtual ~Asset() = default;

		[[nodiscard]] const String& GetName() const { return m_Name; }
		[[nodiscard]] const String& GetFullName() const { return m_FullName; }
		[[nodiscard]] const String& GetExtension() const { return m_Extension; }
		[[nodiscard]] const Path& GetPath() const { return m_Path; }
		[[nodiscard]] const UUID& GetUUID() const { return m_UUID; }
		[[nodiscard]] bool IsDeserialized() const { return m_Deserialized; }

		void Serialize();
		bool Deserialize();
		void SerializeMeta();

		void DrawEditor();
		void Rename(const Path& path)
		{
			m_Name = path.stem().string().c_str();
			m_FullName = path.filename().string().c_str();
			m_Extension = path.extension().string().c_str();
			m_MetaPath = path.string().append(".meta").c_str();
			m_Path = path;
		}

	protected:
		virtual void DrawProperties() { }

		[[nodiscard]] virtual bool OverrideDataFile() const { return true; }
		virtual void SerializeData(YAML::Emitter& out) = 0;
		virtual void DeserializeData(YAML::Node& node) = 0;

		virtual void SerializeMeta(YAML::Emitter& out) { }
		virtual void DeserializeMeta(YAML::Node& node) { }

		String m_Name;
		String m_FullName;
		String m_Extension;
		Path m_MetaPath;
		Path m_Path;
		UUID m_UUID;
		bool m_Deserialized = false;

	private:
		void SerializeData();
		bool DeserializeData();
		bool DeserializeMeta();
	};
}
