#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class MenuBar;

    // Owns every dockable editor window. Any panel type can be opened several times,
    // the set of opened panels is stored in imgui.ini alongside the layout itself
    class PanelManager
    {
    public:
        PanelManager() = default;
        ~PanelManager();

        //Registers the imgui.ini settings handler. Has to be called before the first ImGui frame,
        //but after the ImGui context is created, so it can not be done in the constructor
        void Initialize();

        PanelManager(const PanelManager&) = delete;
        PanelManager& operator=(const PanelManager&) = delete;

        void RegisterPanelType(const String& typeName, const Func<Ref<Panel>>& factory, bool openByDefault);
        void AddMainMenuItems(const Ref<MenuBar>& menuBar);

        //Opens one more instance of a panel type
        void OpenPanel(const String& typeName);
        //Focuses the first opened instance of a panel type or opens a new one
        void OpenOrFocusPanel(const String& typeName);

        void CloseAllPanels();
        void OpenDefaultPanels();

        //Creates the default set of panels if nothing was restored from imgui.ini yet
        void EnsureRestored();

        //True once after the default set of panels was opened, so the layout can be rebuilt
        bool ConsumeDefaultLayoutRequest();

        void Render();

        [[nodiscard]] bool OnKeyPressed(KeyPressedEvent& event) const;

        [[nodiscard]] const Vector<Ref<Panel>>& GetPanels() const { return m_Panels; }
        //Window name of the first opened instance of a panel type. Empty if there is none
        [[nodiscard]] String GetWindowName(const String& typeName) const;

        template <class T> [[nodiscard]] Vector<Ref<T>> GetPanelsOfType() const;
        template <class T> [[nodiscard]] Ref<T> GetFirstPanelOfType() const;

    private:
        struct PanelType
        {
            String Name;
            Func<Ref<Panel>> Factory;
            bool OpenByDefault{};
        };

        struct PanelInstance
        {
            String TypeName;
            int Index{};
            bool FocusOnOpen{};
        };

        [[nodiscard]] const PanelType* FindPanelType(const String& typeName) const;
        void UpdateSavedPanels();
        [[nodiscard]] int GetFreeInstanceIndex(const String& typeName) const;
        void CreatePanel(const String& typeName, int index);
        void ProcessPanelsToOpen();
        void RemoveClosedPanels();

        static void SettingsClearAll(ImGuiContext* context, ImGuiSettingsHandler* handler);
        static void* SettingsReadOpen(ImGuiContext* context, ImGuiSettingsHandler* handler, const char* name);
        static void SettingsReadLine(ImGuiContext* context, ImGuiSettingsHandler* handler, void* entry, const char* line);
        static void SettingsApplyAll(ImGuiContext* context, ImGuiSettingsHandler* handler);
        static void SettingsWriteAll(ImGuiContext* context, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buffer);

        inline static const char* SettingsTypeName = "LevEditorPanels";

        //ImGui writes its settings when the context is destroyed, which happens after the
        //manager is gone, so the handler works with these instead of the manager itself
        inline static PanelManager* s_Instance = nullptr;
        inline static Vector<PanelInstance> s_SavedPanels;

        Vector<PanelType> m_PanelTypes;
        Vector<Ref<Panel>> m_Panels;
        Vector<PanelInstance> m_PanelsToOpen;
        Vector<PanelInstance> m_RestoredPanels;

        bool m_IsInitialized = false;
        bool m_IsRestored = false;
        bool m_IsDefaultLayoutRequested = false;
    };
}

#include "PanelManager.inl"
