#include "pch.h"
#include "Asset.h"

#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"

namespace LevEngine
{
	void Asset::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		Serialize(out);

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
			return Deserialize(data);
		}
		catch (YAML::ParserException& e)
		{
			Log::CoreWarning(e.what());
			return false;
		}
	}
}
