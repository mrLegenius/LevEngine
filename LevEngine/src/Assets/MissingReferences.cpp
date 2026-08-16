#include "levpch.h"
#include "MissingReferences.h"

#include "AssetDatabase.h"
#include "Project.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
	namespace
	{
		//<--- The file and the path inside it a failed lookup is blamed on. Components are
		//deserialized by a parallel job, so every thread carries its own ---<<
		thread_local String t_Source;
		thread_local Vector<String> t_Location;

		const String k_UnknownSource = "Unknown";

		//<--- Entity ids, parents and asset UUIDs are all uint64 in YAML. These keys never hold an
		//asset reference, so they are never followed ---<<
		bool IsNotAReferenceKey(const String& key)
		{
			return key == "Entity" || key == "Parent" || key == "UUID" || key == "RootEntity";
		}

		bool IsScannableAsset(const Path& path)
		{
			return AssetDatabase::IsAssetScene(path)
				|| AssetDatabase::IsAssetPrefab(path)
				|| AssetDatabase::IsAssetMaterial(path)
				|| AssetDatabase::IsAssetPBRMaterial(path)
				|| AssetDatabase::IsAssetCustomMaterial(path)
				|| AssetDatabase::IsAssetSkybox(path)
				|| AssetDatabase::IsAssetPlanetBiomeSet(path);
		}

		String JoinLocation(const String& location, const String& part)
		{
			if (location.empty()) return part;

			return location + " / " + part;
		}
	}

	MissingReferences::SourceScope::SourceScope(const String& source)
		: m_Previous(t_Source)
		, m_PreviousLocationDepth(t_Location.size())
	{
		t_Source = source;
	}

	MissingReferences::SourceScope::~SourceScope()
	{
		t_Source = m_Previous;

		//<--- A source that ends while a location is still open would leak the path into whatever
		//is deserialized next on this thread ---<<
		while (t_Location.size() > m_PreviousLocationDepth)
			t_Location.pop_back();
	}

	MissingReferences::LocationScope::LocationScope(const String& part)
		: m_Pushed(!part.empty())
	{
		if (m_Pushed)
			t_Location.push_back(part);
	}

	MissingReferences::LocationScope::~LocationScope()
	{
		if (m_Pushed && !t_Location.empty())
			t_Location.pop_back();
	}

	String MissingReferences::GetCurrentLocation()
	{
		String location;

		for (const auto& part : t_Location)
			location = JoinLocation(location, part);

		return location;
	}

	void MissingReferences::Report(const UUID reference)
	{
		Report(reference, t_Source.empty() ? k_UnknownSource : t_Source, GetCurrentLocation());
	}

	void MissingReferences::Report(const UUID reference, const String& source, const String& location)
	{
		{
			std::lock_guard lock(s_Mutex);

			for (const auto& existing : s_References)
			{
				if (existing.Reference == reference
					&& existing.Source == source
					&& existing.Location == location)
					return;
			}

			s_References.push_back(MissingReference{ reference, source, location });
		}

		s_Version++;

		if (s_IsScanning) return;

		//<--- Outside the lock, the log has one of its own ---<<
		if (location.empty())
		{
			Log::CoreWarning("Missing asset {0} referenced by '{1}'",
				static_cast<std::uint64_t>(reference), source);
		}
		else
		{
			Log::CoreWarning("Missing asset {0} referenced by '{1}' ({2})",
				static_cast<std::uint64_t>(reference), source, location);
		}
	}

	void MissingReferences::Clear()
	{
		{
			std::lock_guard lock(s_Mutex);
			s_References.clear();
		}

		s_Version++;
	}

	uint32_t MissingReferences::GetVersion() { return s_Version; }

	Vector<MissingReference> MissingReferences::GetAll()
	{
		std::lock_guard lock(s_Mutex);

		return s_References;
	}

	size_t MissingReferences::GetCount()
	{
		std::lock_guard lock(s_Mutex);

		return s_References.size();
	}

	bool MissingReferences::IsResourcesDatabaseSource(const String& source)
	{
		return source == ResourcesDatabaseSource;
	}

	void MissingReferences::ScanProject()
	{
		Clear();

		s_IsScanning = true;

		const auto assetsRoot = AssetDatabase::GetAssetsPath();

		std::error_code errorCode;
		if (exists(assetsRoot, errorCode))
		{
			for (std::filesystem::recursive_directory_iterator i(assetsRoot, errorCode), end; i != end; ++i)
			{
				if (!i->is_regular_file()) continue;

				if (!IsScannableAsset(i->path())) continue;

				ScanAssetFile(i->path());
			}
		}

		ScanResourcesDatabase();

		s_IsScanning = false;

		Log::CoreInfo("Scanned project for missing references, {0} found", GetCount());
	}

	void MissingReferences::ScanAssetFile(const Path& path)
	{
		YAML::Node data;
		if (!LoadYAMLFileSafe(path, data)) return;

		std::error_code errorCode;
		auto relativePath = relative(path, AssetDatabase::GetAssetsPath(), errorCode);
		const String source = errorCode
			? String(path.string().c_str())
			: String(relativePath.generic_string().c_str());

		if (const auto entities = data["Entities"])
		{
			CollectEntityIds(entities);
			ScanEntities(entities, source);
			s_ScannedEntityIds.clear();
		}

		//<--- A scene names the script systems it runs, and those are assets like any other ---<<
		if (const auto systems = data["Systems"])
		{
			for (const auto& system : systems)
				ScanNode(system["System"], source, "Systems");
		}

		if (!data["Entities"])
			ScanNode(data, source, String{});
	}

	void MissingReferences::CollectEntityIds(const YAML::Node& entities)
	{
		s_ScannedEntityIds.clear();

		for (const auto& entity : entities)
		{
			const auto id = entity["Entity"];
			if (!id) continue;

			try
			{
				s_ScannedEntityIds.insert(id.as<std::uint64_t>());
			}
			catch (std::exception&)
			{
			}
		}
	}

	void MissingReferences::ScanEntities(const YAML::Node& entities, const String& source)
	{
		for (const auto& entity : entities)
		{
			if (!entity.IsMap()) continue;

			String tag;
			if (const auto tagNode = entity["Tag"])
				tag = tagNode.as<String>();

			for (const auto& component : entity)
			{
				const auto key = component.first.as<String>();

				if (key == "Tag" || IsNotAReferenceKey(key)) continue;

				ScanNode(component.second, source, Format("Entity '{0}' / {1}", tag, key));
			}
		}
	}

	void MissingReferences::ScanNode(const YAML::Node& node, const String& source, const String& location)
	{
		if (!node) return;

		switch (node.Type())
		{
		case YAML::NodeType::Scalar:
			{
				UUID reference{ 0 };
				if (!TryReadReference(node, reference)) return;

				if (AssetDatabase::HasAsset(reference)) return;

				Report(reference, source, location);
				return;
			}

		case YAML::NodeType::Sequence:
			{
				int index = 0;
				for (const auto& element : node)
				{
					ScanNode(element, source, Format("{0}[{1}]", location, index));
					index++;
				}
				return;
			}

		case YAML::NodeType::Map:
			{
				for (const auto& pair : node)
				{
					const auto key = pair.first.as<String>();

					if (IsNotAReferenceKey(key)) continue;

					ScanNode(pair.second, source, JoinLocation(location, key));
				}
				return;
			}

		default:
			return;
		}
	}

	void MissingReferences::ScanResourcesDatabase()
	{
		const auto path = Project::GetRoot() / ResourcesDatabaseSource.c_str();

		std::error_code errorCode;
		if (!exists(path, errorCode)) return;

		YAML::Node data;
		if (!LoadYAMLFileSafe(path, data)) return;

		const auto assets = data["Assets"];
		if (!assets) return;

		for (const auto& asset : assets)
		{
			const auto uuidNode = asset["UUID"];
			const auto addressNode = asset["Address"];

			if (!uuidNode || !addressNode) continue;

			try
			{
				const UUID uuid = uuidNode.as<std::uint64_t>();
				if (AssetDatabase::HasAsset(uuid)) continue;

				Report(uuid, ResourcesDatabaseSource, Format("Address '{0}'", addressNode.as<String>()));
			}
			catch (std::exception&)
			{
			}
		}
	}

	bool MissingReferences::TryReadReference(const YAML::Node& node, UUID& outReference)
	{
		const auto text = node.Scalar();

		//<--- A UUID is a random 64 bit number, so anything short enough to be an ordinary field
		//value -- a count, a flag, an index -- is not one ---<<
		if (text.size() < 10 || text.size() > 20) return false;

		for (const auto character : text)
		{
			if (character < '0' || character > '9') return false;
		}

		std::uint64_t value = 0;
		try
		{
			value = std::stoull(text);
		}
		catch (std::exception&)
		{
			return false;
		}

		if (value <= 0xFFFFFFFFull) return false;

		if (s_ScannedEntityIds.find(value) != s_ScannedEntityIds.end()) return false;

		outReference = UUID(value);

		return true;
	}
}
