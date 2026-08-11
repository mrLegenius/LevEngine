#include "levpch.h"
#include "ClassCollection.h"

namespace LevEngine::Detail
{
	void*& GetClassCollectionSlot(const char* kind, const char* baseTypeName)
	{
		static std::map<std::string, void*> slots;
		static std::mutex mutex;

		std::lock_guard lock(mutex);

		// std::map never invalidates references, so handing one out past the lock is safe.
		return slots[std::string(kind) + '|' + baseTypeName];
	}
}
