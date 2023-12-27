#pragma once

#include "LayerStack.h"
#include "Statistic.h"

#include "VGJS.h"

namespace LevEngine
{
    class Event;
    class Renderer;
    class WindowResizedEvent;
    class WindowClosedEvent;
    class KeyReleasedEvent;
    class KeyPressedEvent;
    class MouseScrolledEvent;
    class MouseButtonReleasedEvent;
    class MouseButtonPressedEvent;
    class MouseMovedEvent;
    class Physics;
    class ImGuiLayer;
    class Window;

    namespace Scripting
    {
        class ScriptingManager;
    }

    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](const int index) const
        {
            LEV_CORE_ASSERT(index < Count);
            return Args[index];
        }
    };

    struct ApplicationSpecification
    {
        String Name = "LevApp";
        uint32_t WindowWidth = 1600;
        uint32_t WindowHeight = 900;
        ApplicationCommandLineArgs CommandLineArgs;
    };

    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& specification);
        ~Application();

        void Run();
        void GameLoop();
        void Render();
        void Close();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void OnEvent(Event& e);
        [[nodiscard]] ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

        [[nodiscard]] Window& GetWindow() const { return *m_Window; }
        [[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }

        static Application& Get() { return *s_Instance; }
        [[nodiscard]] Physics& GetPhysics() const;
        [[nodiscard]] static Renderer& Renderer();
        [[nodiscard]] Scripting::ScriptingManager& GetScriptingManager();

        [[nodiscard]] Statistic GetFrameStat() const;
    private:
        bool OnWindowClosed(WindowClosedEvent& e);
        bool OnWindowResized(WindowResizedEvent& e);
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnKeyReleased(KeyReleasedEvent& e);
        bool OnMouseMoved(MouseMovedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
        bool OnMouseScrolled(MouseScrolledEvent& e);

        Scope<Window> m_Window;
        bool m_IsRunning = true;

        Scope<Physics> m_Physics;
        Scope<LevEngine::Renderer> m_Renderer;

        LayerStack m_LayerStack;

        Statistic m_FrameStat;

        float m_LastFrameTime = 0.0f;
        bool m_Minimized = false;
        ImGuiLayer* m_ImGuiLayer;
        ApplicationSpecification m_Specification;

        static Application* s_Instance;

        vgjs::JobSystem* m_JobSystem;

        Scope<Scripting::ScriptingManager> m_ScriptingManager;
    };

    using App = Application;
}
