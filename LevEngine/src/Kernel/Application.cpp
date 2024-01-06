#include "levpch.h"
#include "Application.h"

#include "Utils.h"
#include "Window.h"
#include "../Renderer/Renderer.h"
#include "../Events/ApplicationEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Input/Input.h"
#include "../Events/Event.h"
#include "Audio/Audio.h"
#include "GUI/ImGuiLayer.h"
#include "Math/Random.h"
#include "Physics/Physics.h"
#include "Platform/D3D11/D3D11DeferredContexts.h"
#include "Renderer/RenderDebugEvent.h"
#include "Renderer/RendererContext.h"
#include "Renderer/RenderTarget.h"
#include "Scene/Scene.h"
#include "Time/Time.h"
#include "Time/TimelineRunner.h"
#include "Scripting/ScriptingManager.h"
#include "Scene/SceneManager.h"

namespace LevEngine
{
    Application* Application::s_Instance = nullptr;


    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification),
          m_JobSystem(new vgjs::JobSystem(vgjs::thread_count_t(0), vgjs::thread_index_t(1)))
    {
        LEV_PROFILE_FUNCTION();

        LEV_CORE_ASSERT(!s_Instance, "Only one application is allowed");
        s_Instance = this;

        m_Window = Window::Create(WindowAttributes(specification.Name, specification.WindowWidth,
                                                   specification.WindowHeight));
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_Renderer = CreateScope<LevEngine::Renderer>(*m_Window);

        m_ImGuiLayer = new ImGuiLayer;
        PushOverlay(m_ImGuiLayer);

        m_Physics = Physics::Create();
        m_ScriptingManager = CreateScope<Scripting::ScriptingManager>();

        Random::Init();
        m_ScriptingManager->Init();
        Audio::Init(Audio::MaxAudioChannelCount, FMOD_STUDIO_INIT_LIVEUPDATE,
                    FMOD_INIT_VOL0_BECOMES_VIRTUAL | FMOD_INIT_3D_RIGHTHANDED);

        Time::s_StartupTime = std::chrono::high_resolution_clock::now();
    }

    Application::~Application()
    {
        Audio::Shutdown();
        m_ScriptingManager->Shutdown();
        SceneManager::Shutdown();
        delete m_JobSystem;
    }

    void Application::Run()
    {
        vgjs::schedule(vgjs::Function{[=]
        {
            D3D11DeferredContexts::Init(m_JobSystem->get_thread_count().value);
            AttachAllLayers();
            GameLoop();
        }, vgjs::thread_index_t{0}});

        m_JobSystem->thread_task();
        vgjs::wait_for_termination();
    }

    void Application::GameLoop()
    {
        std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
        float totalTime = 0;
        unsigned int frameCount = 0;

        Time::Init(1 / 60.0f);

        while (m_IsRunning)
        {
            auto curTime = std::chrono::steady_clock::now();
            float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() /
                1000000.0f;
            Time::SetDeltaTime(deltaTime);
            PrevTime = curTime;

            TimelineRunner::OnUpdate(deltaTime);

            totalTime += deltaTime;
            frameCount++;

            m_FrameStat.Sample(deltaTime * 1000);

            if (totalTime > 1.0f)
            {
                totalTime -= 1.0f;
                frameCount = 0;

                m_FrameStat.Reset();
            }

            if (deltaTime > 1.0f) // Maybe breakpoint is hit
                deltaTime = 1.0f / 60.0f;

            m_Window->HandleInput();

            if (!m_Minimized)
            {
                LEV_PROFILE_SCOPE("LayerStack OnUpdate");
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(deltaTime);
            }

            Audio::Update();

            if (!m_Minimized)
                Render();

            m_Window->Update();

            Input::Reset();
            Time::s_FrameNumber++;
        }
    }

    void Application::Render()
    {
        {
            LEV_PROFILE_SCOPE("LayerStack OnRender");
            for (Layer* layer : m_LayerStack)
                layer->OnRender();
        }

        //We need to bind main render target before drawing GUI
        //TODO: Maybe move to another place
        m_Window->GetContext()->GetRenderTarget()->Bind();

        RenderDebugEvent::Start("GUI");
        m_ImGuiLayer->Begin();
        {
            LEV_PROFILE_SCOPE("LayerStack OnGUIRender");
            for (Layer* layer : m_LayerStack)
                layer->OnGUIRender();
        }
        m_ImGuiLayer->End();
        RenderDebugEvent::End();
    }

    void Application::Close()
    {
        m_IsRunning = false;
        vgjs::terminate();
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::AttachAllLayers()
    {
        for (const auto stack : m_LayerStack)
        {
            stack->OnAttach();
        }
    }

    void Application::PushOverlay(Layer* overlay)
    {
        LEV_PROFILE_FUNCTION();

        m_LayerStack.PushOverlay(overlay);
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowClosedEvent>(BIND_EVENT_FN(Application::OnWindowClosed));
        dispatcher.Dispatch<WindowResizedEvent>(BIND_EVENT_FN(Application::OnWindowResized));
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Application::OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Application::OnKeyReleased));
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Application::OnMouseMoved));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Application::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(Application::OnMouseButtonReleased));
        dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Application::OnMouseScrolled));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.handled)
                break;
        }
    }

    Physics& Application::GetPhysics() const { return *m_Physics; }
    Renderer& Application::Renderer() { return *Get().m_Renderer; }

    Scripting::ScriptingManager& Application::GetScriptingManager()
    {
        return *m_ScriptingManager;
    }

    Statistic Application::GetFrameStat() const { return m_FrameStat; }

    bool Application::OnWindowClosed(WindowClosedEvent& e)
    {
        Close();
        return true;
    }

    bool Application::OnWindowResized(WindowResizedEvent& e)
    {
        const auto height = e.GetHeight();
        const auto width = e.GetWidth();

        if (width == 0 || height == 0)
        {
            m_Minimized = true;

            return false;
        }

        m_Window->GetContext()->ResizeBackBuffer(width, height);

        if (width == 0 || height == 0) return false;

        SceneManager::GetActiveScene()->OnViewportResized(width, height);
        m_Renderer->SetViewport(static_cast<float>(width), static_cast<float>(height));

        m_Minimized = false;

        return false;
    }

    bool Application::OnKeyPressed(KeyPressedEvent& e)
    {
        Input::OnKeyPressed(e.GetKeyCode());

        return false;
    }

    bool Application::OnKeyReleased(KeyReleasedEvent& e)
    {
        Input::OnKeyReleased(e.GetKeyCode());
        return false;
    }

    bool Application::OnMouseMoved(MouseMovedEvent& e)
    {
        Input::OnMouseMoved(e.GetX(), e.GetY());
        return false;
    }

    bool Application::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        Input::OnMouseButtonPressed(e.GetMouseButton());
        return false;
    }

    bool Application::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
    {
        Input::OnMouseButtonReleased(e.GetMouseButton());
        return false;
    }

    bool Application::OnMouseScrolled(MouseScrolledEvent& e)
    {
        Input::OnMouseScrolled(e.GetYOffset());
        return false;
    }
}
