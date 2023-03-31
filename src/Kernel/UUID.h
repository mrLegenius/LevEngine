#pragma once
#include <functional>

namespace LevEngine
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;
		UUID& operator=(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<LevEngine::UUID>
	{
		std::size_t operator()(const LevEngine::UUID& uuid) const noexcept
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}