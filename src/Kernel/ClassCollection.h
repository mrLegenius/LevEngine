#pragma once

namespace LevEngine
{
	template<class TBase>
	class ClassCollection
	{
	public:
		ClassCollection(ClassCollection&&) = delete;
		ClassCollection(const ClassCollection&) = delete;

		static ClassCollection& Instance() {
			static ClassCollection instance;
			return instance;
		}

		template<class TDerived>
		void Register()
		{
			static_assert(std::is_base_of_v<TBase, TDerived>, "TDerived must derive from TBase");
			m_Classes.emplace_back(CreateRef<TDerived>());
		}

		auto begin() { return m_Classes.begin(); }
		auto end() { return m_Classes.end(); }
		auto cbegin() const { return m_Classes.cbegin(); }
		auto cend() const { return m_Classes.cend(); }
		auto begin() const { return m_Classes.begin(); }
		auto end() const { return m_Classes.end(); }

	private:
		ClassCollection() = default;

		std::vector<Ref<TBase>> m_Classes;
	};

	template<class TBase, class TDerived>
	class ClassRegister {
	public:
		ClassRegister()
		{
			static_assert(std::is_base_of_v<TBase, TDerived>, "TDerived must derive from TBase to be registered");
			ClassCollection<TBase>::Instance().Register<TDerived>();
		}
	};
}