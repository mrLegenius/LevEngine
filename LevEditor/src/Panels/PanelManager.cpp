#include "pch.h"
#include "PanelManager.h"

#include "Essentials/MenuBar.h"

namespace LevEngine::Editor
{
    void PanelManager::Initialize()
    {
        if (m_IsInitialized) return;

        LEV_CORE_ASSERT(ImGui::GetCurrentContext(), "ImGui context is not created yet")
        LEV_CORE_ASSERT(s_Instance == nullptr, "Only one panel manager is allowed")

        m_IsInitialized = true;
        s_Instance = this;

        ImGuiSettingsHandler handler;
        handler.TypeName = SettingsTypeName;
        handler.TypeHash = ImHashStr(SettingsTypeName);
        handler.ClearAllFn = SettingsClearAll;
        handler.ReadOpenFn = SettingsReadOpen;
        handler.ReadLineFn = SettingsReadLine;
        handler.ApplyAllFn = SettingsApplyAll;
        handler.WriteAllFn = SettingsWriteAll;

        ImGui::AddSettingsHandler(&handler);
    }

    PanelManager::~PanelManager()
    {
        //The handler is left registered on purpose: ImGui saves the settings when the context
        //is destroyed and the opened panels have to end up in imgui.ini even then
        if (s_Instance == this)
            s_Instance = nullptr;
    }

    void PanelManager::RegisterPanelType(const String& typeName, const Func<Ref<Panel>>& factory, const bool openByDefault)
    {
        LEV_CORE_ASSERT(FindPanelType(typeName) == nullptr, "Panel type is already registered")

        m_PanelTypes.emplace_back(PanelType{ typeName, factory, openByDefault });
    }

    void PanelManager::AddMainMenuItems(const Ref<MenuBar>& menuBar)
    {
        for (const auto& type : m_PanelTypes)
        {
            const String typeName = type.Name;
            menuBar->AddMenuItem(Format("Window/{}", typeName.c_str()), String(),
                                 [this, typeName] { OpenOrFocusPanel(typeName); });
        }

        for (const auto& type : m_PanelTypes)
        {
            const String typeName = type.Name;
            menuBar->AddMenuItem(Format("Window/New Window/{}", typeName.c_str()), String(),
                                 [this, typeName] { OpenPanel(typeName); });
        }
    }

    void PanelManager::OpenPanel(const String& typeName)
    {
        if (!FindPanelType(typeName))
        {
            Log::CoreWarning("Trying to open an unknown panel type '{}'", typeName);
            return;
        }

        m_PanelsToOpen.emplace_back(PanelInstance{ typeName, GetFreeInstanceIndex(typeName), true });
    }

    void PanelManager::OpenOrFocusPanel(const String& typeName)
    {
        for (const auto& panel : m_Panels)
        {
            if (panel->GetTypeName() != typeName) continue;

            panel->Focus();
            return;
        }

        OpenPanel(typeName);
    }

    void PanelManager::CloseAllPanels()
    {
        for (const auto& panel : m_Panels)
            panel->OnClose();

        m_Panels.clear();
        m_PanelsToOpen.clear();

        UpdateSavedPanels();
        ImGui::MarkIniSettingsDirty();
    }

    void PanelManager::OpenDefaultPanels()
    {
        CloseAllPanels();

        for (const auto& type : m_PanelTypes)
        {
            if (!type.OpenByDefault) continue;

            m_PanelsToOpen.emplace_back(PanelInstance{ type.Name, 0, false });
        }

        m_IsRestored = true;
        m_IsDefaultLayoutRequested = true;
        ProcessPanelsToOpen();
    }

    void PanelManager::EnsureRestored()
    {
        if (m_IsRestored) return;

        if (m_RestoredPanels.empty())
        {
            OpenDefaultPanels();
            return;
        }

        m_IsRestored = true;

        for (const auto& instance : m_RestoredPanels)
            CreatePanel(instance.TypeName, instance.Index);

        m_RestoredPanels.clear();
    }

    bool PanelManager::ConsumeDefaultLayoutRequest()
    {
        const bool isRequested = m_IsDefaultLayoutRequested;
        m_IsDefaultLayoutRequested = false;
        return isRequested;
    }

    void PanelManager::Render()
    {
        LEV_PROFILE_FUNCTION();

        ProcessPanelsToOpen();

        //Panels are never added to the collection while rendering, opening goes through m_PanelsToOpen
        for (size_t i = 0; i < m_Panels.size(); ++i)
            m_Panels[i]->Render();

        RemoveClosedPanels();
    }

    bool PanelManager::OnKeyPressed(KeyPressedEvent& event) const
    {
        for (const auto& panel : m_Panels)
        {
            if (panel->OnKeyPressed(event))
                return true;
        }

        return false;
    }

    String PanelManager::GetWindowName(const String& typeName) const
    {
        for (const auto& panel : m_Panels)
        {
            if (panel->GetTypeName() == typeName)
                return panel->GetWindowName();
        }

        return {};
    }

    const PanelManager::PanelType* PanelManager::FindPanelType(const String& typeName) const
    {
        for (const auto& type : m_PanelTypes)
        {
            if (type.Name == typeName)
                return &type;
        }

        return nullptr;
    }

    int PanelManager::GetFreeInstanceIndex(const String& typeName) const
    {
        for (int index = 0;; ++index)
        {
            bool isTaken = false;

            for (const auto& panel : m_Panels)
                isTaken |= panel->GetTypeName() == typeName && panel->GetInstanceIndex() == index;

            for (const auto& instance : m_PanelsToOpen)
                isTaken |= instance.TypeName == typeName && instance.Index == index;

            if (!isTaken)
                return index;
        }
    }

    void PanelManager::CreatePanel(const String& typeName, const int index)
    {
        const auto type = FindPanelType(typeName);
        if (!type) return;

        auto panel = type->Factory();
        if (!panel) return;

        panel->SetInstance(typeName, index);
        m_Panels.emplace_back(Move(panel));

        UpdateSavedPanels();
        ImGui::MarkIniSettingsDirty();
    }

    void PanelManager::UpdateSavedPanels()
    {
        s_SavedPanels.clear();
        for (const auto& panel : m_Panels)
            s_SavedPanels.emplace_back(PanelInstance{ panel->GetTypeName(), panel->GetInstanceIndex(), false });
    }

    void PanelManager::ProcessPanelsToOpen()
    {
        if (m_PanelsToOpen.empty()) return;

        const auto panelsToOpen = m_PanelsToOpen;
        m_PanelsToOpen.clear();

        for (const auto& instance : panelsToOpen)
        {
            const auto panelsCount = m_Panels.size();
            CreatePanel(instance.TypeName, instance.Index);

            if (instance.FocusOnOpen && m_Panels.size() > panelsCount)
                m_Panels.back()->Focus();
        }
    }

    void PanelManager::RemoveClosedPanels()
    {
        for (auto it = m_Panels.begin(); it != m_Panels.end();)
        {
            if ((*it)->IsOpen())
            {
                ++it;
                continue;
            }

            (*it)->OnClose();
            it = m_Panels.erase(it);

            UpdateSavedPanels();
            ImGui::MarkIniSettingsDirty();
        }
    }

    void PanelManager::SettingsClearAll(ImGuiContext* context, ImGuiSettingsHandler* handler)
    {
        if (!s_Instance) return;

        s_Instance->m_RestoredPanels.clear();
    }

    void* PanelManager::SettingsReadOpen(ImGuiContext* context, ImGuiSettingsHandler* handler, const char* name)
    {
        //Has to be non null for the lines of the entry to be read
        return handler;
    }

    void PanelManager::SettingsReadLine(ImGuiContext* context, ImGuiSettingsHandler* handler, void* entry, const char* line)
    {
        if (!s_Instance) return;

        char typeName[128];
        int index;
        if (sscanf_s(line, "Panel=%127[^,],%d", typeName, static_cast<unsigned>(sizeof typeName), &index) == 2)
            s_Instance->m_RestoredPanels.emplace_back(PanelInstance{ typeName, index, false });
    }

    void PanelManager::SettingsApplyAll(ImGuiContext* context, ImGuiSettingsHandler* handler)
    {
        if (!s_Instance) return;

        s_Instance->EnsureRestored();
    }

    void PanelManager::SettingsWriteAll(ImGuiContext* context, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buffer)
    {
        buffer->appendf("[%s][Panels]\n", handler->TypeName);
        for (const auto& panel : s_SavedPanels)
            buffer->appendf("Panel=%s,%d\n", panel.TypeName.c_str(), panel.Index);

        buffer->append("\n");
    }
}
