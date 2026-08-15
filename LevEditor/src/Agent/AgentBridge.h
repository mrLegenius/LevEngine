#pragma once

#include "AgentServer.h"
#include "Panels/ConsoleLog.h"

namespace LevEngine
{
    class Texture;
    class EditorCamera;
}

namespace LevEngine::Editor
{
    class EditorLayer;
    class ViewportPanel;
    class JsonWriter;

    // Everything an agent can do to the editor from outside it.
    //
    // The editor is driven by a mouse: the camera only moves while a button is held, values are typed
    // into an inspector, the statistics panel can only be read by looking at it. None of that can be
    // done by a program, so anything automated ended up faking it -- moving a planet instead of the
    // camera, printing timings into the log and grepping them, editing scene YAML by hand and
    // restarting. This is the same editor with a second way in.
    //
    // Commands run on the main thread out of AgentServer's queue. A command that cannot answer in one
    // frame (a camera flight, a screenshot that has to wait for the next render) holds on to its call
    // and answers later.
    class AgentBridge
    {
    public:
        explicit AgentBridge(EditorLayer& editor);
        ~AgentBridge();

        //<--- Port comes from LEV_AGENT_PORT, and LEV_AGENT_DISABLE=1 turns the whole thing off ---<<
        void Start();
        void Stop();

        //<--- Main thread, once a frame ---<<
        void Tick(float deltaTime);

        //<--- Main thread, right after the scene has been rendered into the given texture ---<<
        void OnViewportRendered(const Ref<Texture>& texture);

        //<--- The same, for what the game panel renders through the scene's own camera ---<<
        void OnGameRendered(const Ref<Texture>& texture);

    private:
        using CommandHandler = String (AgentBridge::*)(const YAML::Node&, const Ref<AgentServer::Call>&);

        struct Command
        {
            const char* Name;
            const char* Description;
            CommandHandler Handler;
        };

        //<--- A flight along a set of waypoints, one sample per requested interval ---<<
        struct CameraPath
        {
            Vector<Vector3> Waypoints;
            Vector3 LookAtTarget;
            bool HasLookAtTarget = false;

            float Duration = 1.0f;
            float SampleInterval = 0.1f;

            float Elapsed = 0.0f;
            float TimeSinceSample = 0.0f;

            //<--- Every sample, already written as JSON ---<<
            Vector<String> Samples;

            Ref<AgentServer::Call> Call;
        };

        struct PendingScreenshot
        {
            Path SavePath;
            //<--- The game panel's camera rather than the viewport's ---<<
            bool WantsGame = false;
            int Width = 0;
            int Height = 0;
            int FramesWaited = 0;

            Ref<AgentServer::Call> Call;
        };

        String Dispatch(const Ref<AgentServer::Call>& call);

        void TickCameraPath(float deltaTime);
        void TickPendingScreenshot();
        void CapturePendingScreenshot(const Ref<Texture>& texture);

        //<--- Shared by editor_stats and by every sample of a camera path ---<<
        void WriteStats(JsonWriter& writer) const;
        void WritePlanetStats(JsonWriter& writer) const;

        [[nodiscard]] ViewportPanel* GetActiveViewport() const;
        [[nodiscard]] EditorCamera* GetActiveCamera() const;

        // Play mode focuses the game panel, which leaves the viewport hidden behind it in the same
        // dock and its camera unreachable. Opening it takes a frame, so the caller is told to try
        // again rather than given a camera that is not the one being drawn.
        [[nodiscard]] String RequireViewportError();

        //<--- Accepts a UUID, an entt id or a tag, because a caller has whichever it got last ---<<
        [[nodiscard]] static Entity FindEntity(const YAML::Node& node, String& outError);

        static String Ok();
        static String Ok(const String& fields);
        static String Error(const String& message);

        // Commands
        String CommandHelp(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandEditorState(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandStats(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandRenderStats(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandCameraGet(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandCameraSet(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandCameraFocus(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandCameraPath(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandScreenshot(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandPlay(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandStop(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandPause(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandResume(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandStep(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandEntityList(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandEntityCreate(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandEntityDestroy(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandComponentList(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandComponentGet(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandComponentSet(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandComponentAdd(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandComponentRemove(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandSceneOpen(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandSceneSave(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandSceneReload(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandShaderReload(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandShaderValidateLayout(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        String CommandLogTail(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);
        String CommandQuit(const YAML::Node& arguments, const Ref<AgentServer::Call>& call);

        static const Vector<Command>& GetCommands();

        EditorLayer& m_Editor;

        AgentServer m_Server;

        Scope<CameraPath> m_CameraPath;
        Scope<PendingScreenshot> m_PendingScreenshot;

        float m_LastDeltaTime = 0.0f;

        Vector<ConsoleLog::Item> m_LogItems;
        ConsoleLog::Cursor m_LogCursor;
    };
}
