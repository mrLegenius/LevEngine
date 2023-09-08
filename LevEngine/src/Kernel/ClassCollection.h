#pragma once
#include "DataTypes/Vector.h"
#include "Kernel/PointerUtils.h"

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

		Vector<Ref<TBase>> m_Classes;
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

	template<class TBase>
	class OrderedClassCollection
	{
	public:
		OrderedClassCollection(OrderedClassCollection&&) = delete;
		OrderedClassCollection(const OrderedClassCollection&) = delete;

		static OrderedClassCollection& Instance() {
			static OrderedClassCollection instance;
			return instance;
		}

		template<class TDerived, int Order>
		void Register()
		{
			static_assert(std::is_base_of_v<TBase, TDerived>, "TDerived must derive from TBase");

			auto element = std::make_pair(CreateRef<TDerived>(), Order);

			for (auto it = m_Classes.begin(); it != m_Classes.end(); ++it)
			{
				if (it->second <= Order) continue;

				m_Classes.insert(it, element);
				return;
			}

			m_Classes.emplace_back(element);
		}

		auto begin() { return m_Classes.begin(); }
		auto end() { return m_Classes.end(); }
		auto cbegin() const { return m_Classes.cbegin(); }
		auto cend() const { return m_Classes.cend(); }
		auto begin() const { return m_Classes.begin(); }
		auto end() const { return m_Classes.end(); }

	private:
		OrderedClassCollection() = default;

		Vector<std::pair<Ref<TBase>, int>> m_Classes;
	};

	template<class TBase, class TDerived, int Order>
	class OrderedClassRegister {
	public:
		OrderedClassRegister()
		{
			static_assert(std::is_base_of_v<TBase, TDerived>, "TDerived must derive from TBase to be registered");
			OrderedClassCollection<TBase>::Instance().template Register<TDerived, Order>();
		}
	};
}