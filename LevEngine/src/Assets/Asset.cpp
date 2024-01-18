#include "levpch.h"
#include "Asset.h"
#include "EngineAssets.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	Asset::Asset(const Path& path, const UUID uuid): m_Name(path.stem().string().c_str())
	                                                 , m_FullName(path.filename().string().c_str())
	                                                 , m_Extension(path.extension().string().c_str())
	                                                 , m_MetaPath(path.string().append(".meta").c_str())
	                                                 , m_Path(path)
	                                                 , m_UUID(uuid)
	{
	}

	void Asset::Serialize()
	{
		SerializeData();
		SerializeMeta();
	}

	void Asset::SerializeData()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		SerializeData(out);

		out << YAML::EndMap;

		if (!WriteDataToFile()) return;

		try
		{
			std::ofstream fout(m_Path);
			fout << out.c_str();
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to serialize data of '{0}' asset. Error: {1}", m_Name, e.what());
		}
	}

	void Asset::SerializeMeta()
	{
		YAML::Emitter metaOut;
		metaOut << YAML::BeginMap;

		metaOut << YAML::Key << "UUID" << YAML::Value << m_UUID;
		metaOut << YAML::Key << "Address" << YAML::Value << m_Address;
		
		SerializeMeta(metaOut);

		metaOut << YAML::EndMap;

		try
		{
			std::ofstream fout(m_MetaPath);
			fout << metaOut.c_str();
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to serialize meta of '{0}' asset. Error: {1}", m_Name, e.what());
		}
	}

	void Asset::Rename(const Path& path)
	{
		m_Name = path.stem().string().c_str();
		m_FullName = path.filename().string().c_str();
		m_Extension = path.extension().string().c_str();
		m_MetaPath = path.string().append(".meta").c_str();
		m_Path = path;
	}

	Ref<Texture> Asset::GetIcon() const { return Icons::File(); }

	bool Asset::Deserialize(const bool force)
	{
		std::lock_guard lock(m_DeserializationMutex);
		if (m_Deserialized && !force) return true;
		
		m_Deserialized = DeserializeMeta();
		m_Deserialized = DeserializeData();

		return m_Deserialized = true;
	}

	bool Asset::DeserializeData()
	{
		if (!ReadDataFromFile())
		{
			const YAML::Node data{};
			DeserializeData(data);
			return true;
		}
		
		try
		{
			const YAML::Node data = LoadYAMLFile(m_Path);
			DeserializeData(data);
		}
		catch (YAML::BadConversion&)
		{
			Log::CoreWarning("Data of '{0}' asset is corrupted. Regenerating...", m_Name);
			SerializeData();

			return false;
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to deserialize data of '{0}' asset. Error: {1}", m_Name, e.what());
			return false;
		}
		return true;
	}

	bool Asset::DeserializeMeta()
	{
		try
		{
			YAML::Node meta = YAML::LoadFile(m_MetaPath.string());
			if (const auto address = meta["Address"])
				m_Address = address.as<String>();
			
			DeserializeMeta(meta);
		}
		catch (YAML::BadConversion&)
		{
			Log::CoreWarning("Meta of '{0}' asset is corrupted. Regenerating...", m_Name);
			SerializeMeta();

			return false;
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to deserialize meta of '{0}' asset. Error: {1}", m_Name, e.what());
			return false;
		}
		return true;
	}

}
