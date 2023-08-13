﻿#pragma once
#include "yaml-cpp/emitter.h"

namespace LevEngine
{
	class Asset
	{
	public:
		explicit Asset(std::filesystem::path path)
			: m_Name(path.filename().string())
			, m_Extension(path.extension().string())
			, m_Path(std::move(path)) { }
		virtual ~Asset() = default;

		const std::string& GetName() const { return m_Name; }
		const std::string& GetExtension() const { return m_Extension; }

		void Serialize();
		bool Deserialize();
		virtual void DrawProperties() = 0;

	protected:
		virtual void Serialize(YAML::Emitter& out) = 0;
		virtual bool Deserialize(YAML::Node& node) = 0;
		std::string m_Name;
		std::string m_Extension;
		std::filesystem::path m_Path;
	};
}
