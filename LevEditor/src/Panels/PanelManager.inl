#pragma once

namespace LevEngine::Editor
{
    template <class T> Vector<Ref<T>> PanelManager::GetPanelsOfType() const
    {
        Vector<Ref<T>> result;
        for (const auto& panel : m_Panels)
        {
            if (auto casted = CastRef<T>(panel))
                result.emplace_back(Move(casted));
        }

        return result;
    }

    template <class T> Ref<T> PanelManager::GetFirstPanelOfType() const
    {
        for (const auto& panel : m_Panels)
        {
            if (auto casted = CastRef<T>(panel))
                return casted;
        }

        return nullptr;
    }
}
