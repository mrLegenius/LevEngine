﻿#include "levpch.h"
#include "Asset.h"

namespace LevEngine
{
	void Asset::DrawEditor()
	{
		DrawProperties();
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

		if (!OverrideDataFile()) return;

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

	bool Asset::Deserialize()
	{
		m_Deserialized = DeserializeData();
		m_Deserialized = DeserializeMeta();

		return m_Deserialized = true;
	}

	bool Asset::DeserializeData()
	{
		if (!OverrideDataFile())
		{
			YAML::Node data{};
			DeserializeData(data);
			return true;
		}

		try
		{
			YAML::Node data = YAML::LoadFile(m_Path.string());
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
