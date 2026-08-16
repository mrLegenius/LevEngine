#include "levpch.h"
#include "Asset.h"
#include "AssetDatabase.h"
#include "EngineAssets.h"
#include "MissingReferences.h"
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

	void Asset::UpdateLastChangeTime()
	{
		std::error_code errorCode;
		const auto lastChangeTime = last_write_time(m_Path, errorCode);

		if (errorCode) return;

		m_LastChangeTime = lastChangeTime;
	}

	void Asset::Serialize()
	{
		SerializeData();
		SerializeMeta();

		UpdateLastChangeTime();
	}

	void Asset::SerializeData()
	{
		try
		{
			if (!exists(m_Path) && !WriteDataToFile())
			{
				std::ofstream fout(m_Path);
				return;
			}

			if (WriteDataToFile())
			{
				std::ofstream fout(m_Path);
				
				YAML::Emitter out;
				out << YAML::BeginMap;

				SerializeData(out);

				out << YAML::EndMap;
				fout << out.c_str();
			}
		}
		catch (std::exception& e)
		{
			Log::CoreWarning("Failed to serialize data of '{0}' asset. Error: {1}", m_Name, e.what());
		}
	}

	void Asset::SerializeMeta()
	{
		if (!GenerateMeta()) return;

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

		//<--- Anything this asset points at and cannot find is blamed on this asset ---<<
		std::error_code errorCode;
		const auto relativePath = relative(m_Path, AssetDatabase::GetAssetsPath(), errorCode);
		const MissingReferences::SourceScope source(errorCode
			? String(m_Path.string().c_str())
			: String(relativePath.generic_string().c_str()));

		const bool metaDeserialized = DeserializeMeta();
		const bool dataDeserialized = DeserializeData();

		UpdateLastChangeTime();

		//<--- Marked as deserialized even on failure, otherwise a broken asset is parsed again every frame ---<<
		m_Deserialized = true;

		return metaDeserialized && dataDeserialized;
	}

	bool Asset::DeserializeData()
	{
		try
		{
			if (LoadFromCache()) return true;
		}
		catch (std::exception& e)
		{
		}
		
		if (!ReadDataFromFile())
		{
			const YAML::Node data{};
			DeserializeData(data);
			SaveToCache();
			return true;
		}
		
		try
		{
			const YAML::Node data = LoadYAMLFile(m_Path);
			DeserializeData(data);
			SaveToCache();
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
		if (!GenerateMeta()) return true;

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
