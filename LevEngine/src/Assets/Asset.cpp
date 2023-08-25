#include "levpch.h"
#include "Asset.h"

namespace LevEngine
{
	void Asset::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		SerializeData(out);

		out << YAML::EndMap;

		try
		{
			std::ofstream fout(m_Path);
			fout << out.c_str();
		}
		catch (std::exception& e)
		{
			Log::CoreWarning(e.what());
		}

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
			Log::CoreWarning(e.what());
		}
	}

	bool Asset::Deserialize()
	{
		try
		{
			YAML::Node data = YAML::LoadFile(m_Path.string());
			YAML::Node meta = YAML::LoadFile(m_MetaPath.string());
			m_Deserialized = DeserializeData(data) && DeserializeMeta(meta);

			return m_Deserialized;
		}
		catch (std::exception& e)
		{
			Log::CoreWarning(e.what());
			return false;
		}
	}
}
