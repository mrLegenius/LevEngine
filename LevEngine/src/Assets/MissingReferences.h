#pragma once

#include "DataTypes/Set.h"
#include "Kernel/Core.h"
#include "Kernel/UUID.h"

namespace LevEngine
{
	// A UUID something points at that no asset in the database answers to.
	//
	// A dangling reference used to be a bare "Asset with 123456 is not found" in the log, which says
	// neither what is missing nor who is asking for it. Every lookup that fails now lands here
	// together with the file it came from and the place inside that file, so it can be listed,
	// queried by an agent and fixed.
	struct MissingReference
	{
		UUID Reference{ 0 };

		//<--- The file holding the reference, relative to the project ---<<
		String Source;

		//<--- Where inside that file: "Entity 'Enemy' / MeshRenderer / Mesh", "Address 'Dance1'" ---<<
		String Location;
	};

	class LEV_API MissingReferences
	{
	public:
		// While one of these is alive, every failed lookup on this thread is attributed to the given
		// file. Deserialization of entities runs as a parallel job, so the context is per thread and
		// a job has to open its own scope rather than inherit the one the scheduler was called from.
		class LEV_API SourceScope
		{
		public:
			explicit SourceScope(const String& source);
			~SourceScope();

			SourceScope(const SourceScope&) = delete;
			SourceScope& operator=(const SourceScope&) = delete;

		private:
			String m_Previous;
			size_t m_PreviousLocationDepth;
		};

		//<--- One more step of the path inside the current source, joined with ' / ' ---<<
		class LEV_API LocationScope
		{
		public:
			explicit LocationScope(const String& part);
			~LocationScope();

			LocationScope(const LocationScope&) = delete;
			LocationScope& operator=(const LocationScope&) = delete;

		private:
			bool m_Pushed;
		};

		//<--- Reports against whatever context the calling thread is in ---<<
		static void Report(UUID reference);
		static void Report(UUID reference, const String& source, const String& location);

		static void Clear();

		//<--- A copy, because deserialization jobs write to the registry from several threads ---<<
		[[nodiscard]] static Vector<MissingReference> GetAll();
		[[nodiscard]] static size_t GetCount();

		//<--- Changes whenever something is added or the list is cleared, so a panel can tell it
		//has to read the list again without copying it every frame ---<<
		[[nodiscard]] static uint32_t GetVersion();

		// Reads every YAML asset of the project and reports what does not resolve, including assets
		// that were never loaded. Everything found before is dropped, so the result is the state of
		// the project on disk rather than a log of the session.
		static void ScanProject();

		//<--- Set of the addresses in ResourcesDatabase.asset that point at nothing ---<<
		[[nodiscard]] static bool IsResourcesDatabaseSource(const String& source);

		inline static const String ResourcesDatabaseSource = "ResourcesDatabase.asset";

	private:
		static String GetCurrentLocation();

		static void ScanAssetFile(const Path& path);
		static void ScanEntities(const YAML::Node& entities, const String& source);
		static void ScanNode(const YAML::Node& node, const String& source, const String& location);
		static void ScanResourcesDatabase();

		//<--- Entity ids and asset UUIDs are both random 64 bit numbers, so a scan of a scene has to
		//know which ids belong to the entities of that very file before it can call one missing ---<<
		static void CollectEntityIds(const YAML::Node& entities);

		static bool TryReadReference(const YAML::Node& node, UUID& outReference);

		//<--- The context a report is attributed to is per thread and lives in MissingReferences.cpp:
		//exported thread local data is a portability trap and nothing outside the engine needs it ---<<

		inline static std::mutex s_Mutex;
		inline static Vector<MissingReference> s_References;
		inline static std::atomic<uint32_t> s_Version{ 0 };

		//<--- A scan reports everything a project holds at once, and a line per reference would
		//bury the log. The summary it ends with is what a scan says instead ---<<
		inline static bool s_IsScanning = false;

		//<--- Only used by a scan, which runs on the main thread ---<<
		inline static Set<uint64_t> s_ScannedEntityIds;
	};
}
