#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
	namespace Detail
	{
		/// Returns the process-wide slot holding a class collection, keyed by kind + base type name.
		///
		/// The table itself lives in LevEngine.dll. Collections are templates, so without this every
		/// module would get its own function-local static: a serializer or drawer registered from
		/// LevEditor or from a game project would land in that module's collection and the engine
		/// would never see it. Routing the storage through one exported function keeps a single
		/// collection per base type no matter which module first touches it.
		///
		/// Slots are intentionally never freed - collections are reachable from static initializers
		/// in several modules and there is no safe order in which to tear them down.
		LEV_API void*& GetClassCollectionSlot(const char* kind, const char* baseTypeName);
	}

	template<class TBase>
	class ClassCollection
	{
	public:
		ClassCollection(ClassCollection&&) = delete;
		ClassCollection(const ClassCollection&) = delete;

		static ClassCollection& Instance() {
			static ClassCollection& instance = Acquire();
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

		static ClassCollection& Acquire()
		{
			void*& slot = Detail::GetClassCollectionSlot("ClassCollection", typeid(TBase).name());
			if (slot == nullptr)
				slot = new ClassCollection();

			return *static_cast<ClassCollection*>(slot);
		}

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
			static OrderedClassCollection& instance = Acquire();
			return instance;
		}

		template<class TDerived, int Order>
		void Register()
		{
			static_assert(eastl::is_base_of_v<TBase, TDerived>, "TDerived must derive from TBase");

			auto element = MakePair(CreateRef<TDerived>(), Order);

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

		static OrderedClassCollection& Acquire()
		{
			void*& slot = Detail::GetClassCollectionSlot("OrderedClassCollection", typeid(TBase).name());
			if (slot == nullptr)
				slot = new OrderedClassCollection();

			return *static_cast<OrderedClassCollection*>(slot);
		}

		Vector<Pair<Ref<TBase>, int>> m_Classes;
	};

	template<class TBase, class TDerived, int Order>
	class OrderedClassRegister {
	public:
		OrderedClassRegister()
		{
			static_assert(eastl::is_base_of_v<TBase, TDerived>, "TDerived must derive from TBase to be registered");
			OrderedClassCollection<TBase>::Instance().template Register<TDerived, Order>();
		}
	};
}