#include "pch.h"
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
			fout.close();
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
			return DeserializeData(data);
		}
		catch (std::exception& e)
		{
			Log::CoreWarning(e.what());
			return false;
		}
	}
}
