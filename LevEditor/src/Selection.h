#pragma once

namespace LevEngine
{
	class Selection
	{
	public:
		virtual ~Selection() = default;
		virtual void DrawProperties() = 0;

		static const Ref<Selection>& Current() { return m_CurrentSelection; }
		template<class T>
		static const Ref<T>& CurrentAs()
		{
			return CastRef<T>(m_CurrentSelection);
		}
		static void Select(const Ref<Selection>& selection) { m_CurrentSelection = selection; }
		static void Deselect() { m_CurrentSelection = nullptr; }

	private:
		static Ref<Selection> m_CurrentSelection;
	};
}
