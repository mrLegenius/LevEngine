#include "pch.h"
#include "AgentBridge.h"

#include "AgentJson.h"
#include "AgentScreenshot.h"

#include "EditorCamera.h"
#include "EditorLayer.h"
#include "Project.h"
#include "SceneEditor.h"
#include "SceneState.h"
#include "EntitySelection.h"
#include "Selection.h"

#include "Assets/MissingReferences.h"
#include "Assets/ShaderLibrary.h"
#include "Panels/PanelManager.h"
#include "Panels/GamePanel.h"
#include "Panels/PanelTypes.h"
#include "Panels/ViewportPanel.h"
#include "Renderer/RenderStatistics.h"
#include "Renderer/Shader/ShaderDiagnostics.h"
#include "Scene/Components/Transform/Transform.h"

#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <wrl/client.h>

#pragma comment(lib, "d3dcompiler.lib")
//<--- IID_ID3D11ShaderReflection lives here ---<<
#pragma comment(lib, "dxguid.lib")

namespace LevEngine::Editor
{
    namespace
    {
        constexpr uint16_t k_DefaultPort = 17890;

        //<--- A screenshot waits for a frame to be rendered. If no viewport is open there will
        //never be one, and the caller deserves an answer rather than a timeout ---<<
        constexpr int k_MaxScreenshotWaitFrames = 120;

        String GetEnvironmentVariable(const char* name)
        {
            char buffer[256];
            size_t length = 0;

            if (getenv_s(&length, buffer, sizeof buffer, name) != 0 || length == 0)
                return String{};

            return String{ buffer };
        }

        //<--- Relative paths are relative to the project, which is where a caller thinks it is ---<<
        Path ResolvePath(const String& text)
        {
            Path path{ text.c_str() };

            if (path.is_absolute()) return path;

            const auto root = Project::GetRoot();

            return root.empty() ? path : root / path;
        }
    }

    AgentBridge::AgentBridge(EditorLayer& editor) : m_Editor(editor) { }

    AgentBridge::~AgentBridge() { Stop(); }

    void AgentBridge::Start()
    {
        if (GetEnvironmentVariable("LEV_AGENT_DISABLE") == "1")
        {
            Log::CoreInfo("Agent server disabled by LEV_AGENT_DISABLE");
            return;
        }

        uint16_t port = k_DefaultPort;

        if (const auto configured = GetEnvironmentVariable("LEV_AGENT_PORT"); !configured.empty())
        {
            const auto parsed = std::strtoul(configured.c_str(), nullptr, 10);

            if (parsed > 0 && parsed < 65536)
                port = static_cast<uint16_t>(parsed);
        }

        m_Server.Start(port);
    }

    void AgentBridge::Stop()
    {
        //<--- Anything half done answers before the sockets go away ---<<
        if (m_CameraPath)
        {
            AgentServer::Complete(m_CameraPath->Call, Error("the editor stopped before the camera path finished"));
            m_CameraPath.reset();
        }

        if (m_PendingScreenshot)
        {
            AgentServer::Complete(m_PendingScreenshot->Call, Error("the editor stopped before the frame was captured"));
            m_PendingScreenshot.reset();
        }

        m_Server.Stop();
    }

    void AgentBridge::Tick(const float deltaTime)
    {
        if (!m_Server.IsRunning()) return;

        m_LastDeltaTime = deltaTime;

        TickCameraPath(deltaTime);
        TickPendingScreenshot();

        //<--- One command a frame. A command can move the camera or load a scene, and running two of
        //those against the same frame would answer both with a state neither of them asked for ---<<
        if (m_CameraPath || m_PendingScreenshot) return;

        const auto call = m_Server.TryTakePendingCall();
        if (!call) return;

        String response;

        try
        {
            response = Dispatch(call);
        }
        catch (const std::exception& exception)
        {
            response = Error(Format("{0} failed: {1}", call->Tool, exception.what()));
        }
        catch (...)
        {
            response = Error(Format("{0} failed", call->Tool));
        }

        //<--- Empty means the command took the call and will answer later ---<<
        if (!response.empty())
            AgentServer::Complete(call, response);
    }

    String AgentBridge::Dispatch(const Ref<AgentServer::Call>& call)
    {
        YAML::Node arguments;

        if (!call->Arguments.empty())
        {
            try
            {
                //<--- JSON is YAML, so this parses the request body ---<<
                arguments = YAML::Load(call->Arguments.c_str());
            }
            catch (const std::exception& exception)
            {
                return Error(Format("could not parse the arguments: {0}", exception.what()));
            }
        }

        for (const auto& command : GetCommands())
        {
            if (call->Tool != command.Name) continue;

            return (this->*command.Handler)(arguments, call);
        }

        return Error(Format("unknown tool '{0}', call 'help' for the list", call->Tool));
    }

    const Vector<AgentBridge::Command>& AgentBridge::GetCommands()
    {
        static const Vector<Command> commands
        {
            { "help", "Lists every command", &AgentBridge::CommandHelp },
            { "editor_state", "Project, scene, play state and viewport size", &AgentBridge::CommandEditorState },
            { "editor_stats", "Frame time and per pass GPU time of the last frame, and planet counters", &AgentBridge::CommandStats },
            { "editor_render_stats", "Draw calls, triangles and buffers created per pass", &AgentBridge::CommandRenderStats },

            { "editor_camera_get", "Where the viewport camera is", &AgentBridge::CommandCameraGet },
            { "editor_camera_set", "Moves the viewport camera", &AgentBridge::CommandCameraSet },
            { "editor_camera_focus", "Frames an entity in the viewport camera", &AgentBridge::CommandCameraFocus },
            { "editor_camera_path", "Flies the camera through waypoints and samples stats along the way", &AgentBridge::CommandCameraPath },

            { "editor_screenshot", "Saves the viewport, the game panel or the whole window as a PNG", &AgentBridge::CommandScreenshot },

            { "editor_play", "Enters play mode", &AgentBridge::CommandPlay },
            { "editor_stop", "Leaves play mode", &AgentBridge::CommandStop },
            { "editor_pause", "Stops running systems while staying in play mode", &AgentBridge::CommandPause },
            { "editor_resume", "Runs systems again", &AgentBridge::CommandResume },
            { "editor_step", "Runs a fixed number of frames while paused", &AgentBridge::CommandStep },

            { "entity_list", "Entities in the active scene", &AgentBridge::CommandEntityList },
            { "entity_create", "Creates an entity", &AgentBridge::CommandEntityCreate },
            { "entity_destroy", "Destroys an entity", &AgentBridge::CommandEntityDestroy },

            { "component_list", "Every component key the engine can serialize", &AgentBridge::CommandComponentList },
            { "component_get", "Reads a component's fields", &AgentBridge::CommandComponentGet },
            { "component_set", "Writes some of a component's fields", &AgentBridge::CommandComponentSet },
            { "component_add", "Adds a default constructed component", &AgentBridge::CommandComponentAdd },
            { "component_remove", "Removes a component", &AgentBridge::CommandComponentRemove },

            { "editor_undo", "Takes back the last change to the scene", &AgentBridge::CommandUndo },
            { "editor_redo", "Does the last undone change again", &AgentBridge::CommandRedo },

            { "scene_open", "Opens a scene file", &AgentBridge::CommandSceneOpen },
            { "scene_save", "Saves the active scene", &AgentBridge::CommandSceneSave },
            { "scene_reload", "Reloads the active scene from disk", &AgentBridge::CommandSceneReload },

            { "shader_reload", "Recompiles shaders and reports what failed", &AgentBridge::CommandShaderReload },
            { "shader_validate_layout", "Reflects a constant buffer's fields, offsets and sizes", &AgentBridge::CommandShaderValidateLayout },

            { "log_tail", "The last lines of the editor log", &AgentBridge::CommandLogTail },
            { "missing_references", "Asset references that point at nothing, with the file and the place holding them", &AgentBridge::CommandMissingReferences },
            { "editor_quit", "Closes the editor, so the solution can be rebuilt", &AgentBridge::CommandQuit },
        };

        return commands;
    }

    String AgentBridge::Ok() { return String{ R"({"ok":true})" }; }

    String AgentBridge::Ok(const String& fields)
    {
        if (fields.empty()) return Ok();

        return Format("{{\"ok\":true,{0}}}", fields);
    }

    String AgentBridge::Error(const String& message)
    {
        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", false);
        writer.KeyValue("error", message);
        writer.EndObject();

        return writer.Str();
    }

    ViewportPanel* AgentBridge::GetActiveViewport() const
    {
        const auto& panelManager = m_Editor.GetPanelManager();
        if (!panelManager) return nullptr;

        for (const auto& viewport : panelManager->GetPanelsOfType<ViewportPanel>())
        {
            if (viewport->IsActive())
                return viewport.get();
        }

        return nullptr;
    }

    String AgentBridge::RequireViewportError()
    {
        if (GetActiveViewport()) return String{};

        if (const auto& panelManager = m_Editor.GetPanelManager())
            panelManager->OpenOrFocusPanel(PanelTypes::Viewport);

        return Error("the viewport was not open, it has been opened now -- call again");
    }

    EditorCamera* AgentBridge::GetActiveCamera() const
    {
        const auto viewport = GetActiveViewport();

        return viewport ? &viewport->GetCamera() : nullptr;
    }

    // ---- state and statistics -------------------------------------------------------------------

    String AgentBridge::CommandHelp(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.Key("tools").BeginArray();

        for (const auto& command : GetCommands())
        {
            writer.BeginObject();
            writer.KeyValue("name", command.Name);
            writer.KeyValue("description", command.Description);
            writer.EndObject();
        }

        writer.EndArray();
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandEditorState(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);

        writer.KeyValue("project", Project::GetPath().empty() ? String{} : ToString(Project::GetPath()));
        writer.KeyValue("scene", ToString(SceneManager::GetActiveScenePath()));

        const auto state = m_Editor.GetSceneState();
        writer.KeyValue("sceneState", state == SceneState::Play ? "play" : "edit");
        writer.KeyValue("paused", m_Editor.IsPaused());

        writer.Key("viewport").BeginObject();
        if (const auto viewport = GetActiveViewport())
        {
            writer.KeyValue("open", true);
            writer.KeyValue("width", viewport->GetWidth());
            writer.KeyValue("height", viewport->GetHeight());
        }
        else
        {
            writer.KeyValue("open", false);
        }
        writer.EndObject();

        const auto entitySelection = Selection::CurrentAs<EntitySelection>();

        if (entitySelection && entitySelection->Get())
        {
            const Entity selected = entitySelection->Get();

            writer.Key("selection").BeginObject();
            writer.KeyValue("id", static_cast<uint64_t>(selected.GetUUID()));
            writer.KeyValue("tag", selected.GetName());
            writer.EndObject();
        }
        else
        {
            writer.Key("selection").Null();
        }

        writer.EndObject();

        return writer.Str();
    }

    void AgentBridge::WriteStats(JsonWriter& writer) const
    {
        writer.KeyValue("frameMs", m_LastDeltaTime * 1000.0f);
        writer.KeyValue("fps", m_LastDeltaTime > 0.0f ? 1.0f / m_LastDeltaTime : 0.0f);

        writer.KeyValue("frameMsAverage", App::Get().GetFrameStat().GetAverage());
        writer.KeyValue("fpsAverage", App::Get().GetFrameStat().GetPerSecond());

        // The per pass GPU timers are worth reading and not worth trusting on their own: when two
        // adjacent passes are both cheap the driver moves time between them. The total is the number
        // that holds still.
        writer.Key("gpu").BeginObject();

        const auto& timings = App::Renderer().GetLastFrameTimings();

        for (const auto& [name, milliseconds] : timings)
            writer.KeyValue(name, milliseconds);

        writer.EndObject();

        writer.Key("gpuAverage").BeginObject();
        writer.KeyValue("Frame", App::Renderer().GetFrameStatistic().GetAverage());
        writer.KeyValue("Shadow Map", App::Renderer().GetShadowMapStatistic().GetAverage());
        writer.KeyValue("Environment", App::Renderer().GetEnvironmentStatistic().GetAverage());
        writer.KeyValue("Deferred Geometry", App::Renderer().GetDeferredGeometryStatistic().GetAverage());
        writer.KeyValue("Deferred Lighting", App::Renderer().GetDeferredLightingStatistic().GetAverage());
        writer.KeyValue("Deferred Transparent", App::Renderer().GetDeferredTransparentStatistic().GetAverage());
        writer.KeyValue("Post Processing", App::Renderer().GetPostProcessingStatistic().GetAverage());
        writer.KeyValue("Particles", App::Renderer().GetParticlesStatistic().GetAverage());
        writer.KeyValue("Debug", App::Renderer().GetDebugStatistic().GetAverage());
        writer.EndObject();
    }
    
    String AgentBridge::CommandStats(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        WriteStats(writer);
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandRenderStats(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        if (JsonRead::GetBool(arguments, "disable", false))
        {
            RenderStatistics::SetEnabled(false);
            return Ok(String{ R"("counting":false)" });
        }

        const bool wasEnabled = RenderStatistics::IsEnabled();

        //<--- Counting costs a lock per draw, so it only runs once someone has asked for it ---<<
        RenderStatistics::SetEnabled(true);

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyValue("counting", true);

        if (!wasEnabled)
            writer.KeyValue("note", "counting has just been turned on, ask again after a frame has been rendered");

        writer.KeyValue("frameNumber", RenderStatistics::GetLastFrameNumber());

        writer.Key("perPass").BeginArray();

        uint32_t totalDrawCalls = 0;
        uint32_t totalTriangles = 0;

        for (const auto& pass : RenderStatistics::GetLastFrame())
        {
            totalDrawCalls += pass.DrawCalls;
            totalTriangles += pass.Triangles;

            writer.BeginObject();
            writer.KeyValue("name", pass.Name);
            writer.KeyValue("drawCalls", pass.DrawCalls);
            writer.KeyValue("triangles", pass.Triangles);
            writer.KeyValue("vertexBuffersCreated", pass.VertexBuffersCreated);
            writer.KeyValue("indexBuffersCreated", pass.IndexBuffersCreated);
            writer.KeyValue("constantBufferUpdates", pass.ConstantBufferUpdates);
            writer.EndObject();
        }

        writer.EndArray();

        writer.KeyValue("drawCalls", totalDrawCalls);
        writer.KeyValue("triangles", totalTriangles);

        writer.EndObject();

        return writer.Str();
    }

    // ---- camera ---------------------------------------------------------------------------------

    String AgentBridge::CommandCameraGet(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        if (const auto error = RequireViewportError(); !error.empty()) return error;

        const auto camera = GetActiveCamera();
        if (!camera) return Error("no viewport is open");

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyVector3("position", camera->GetTransform().GetWorldPosition());
        writer.KeyVector3("rotation", camera->GetRotationEuler());
        writer.KeyVector3("forward", camera->GetTransform().GetForwardDirection());
        writer.KeyValue("fov", camera->GetFieldOfView());
        writer.KeyValue("near", camera->GetPerspectiveNear());
        writer.KeyValue("far", camera->GetPerspectiveFar());
        writer.KeyValue("moveSpeed", camera->GetMoveSpeed());
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandCameraSet(const YAML::Node& arguments, const Ref<AgentServer::Call>& call)
    {
        if (const auto error = RequireViewportError(); !error.empty()) return error;

        const auto camera = GetActiveCamera();
        if (!camera) return Error("no viewport is open");

        if (Vector3 position; JsonRead::TryGetVector3(arguments, "position", position))
            camera->SetPosition(position);

        if (Vector3 rotation; JsonRead::TryGetVector3(arguments, "rotation", rotation))
            camera->SetRotationEuler(rotation);

        //<--- After the position, so looking at something from a new place works in one call ---<<
        if (Vector3 target; JsonRead::TryGetVector3(arguments, "lookAt", target))
            camera->LookAt(target);

        if (JsonRead::Has(arguments, "fov"))
            camera->SetFieldOfView(JsonRead::GetFloat(arguments, "fov", camera->GetFieldOfView()));

        if (JsonRead::Has(arguments, "moveSpeed"))
            camera->SetMoveSpeed(JsonRead::GetFloat(arguments, "moveSpeed", camera->GetMoveSpeed()));

        return CommandCameraGet(arguments, call);
    }

    String AgentBridge::CommandCameraFocus(const YAML::Node& arguments, const Ref<AgentServer::Call>& call)
    {
        if (const auto error = RequireViewportError(); !error.empty()) return error;

        const auto camera = GetActiveCamera();
        if (!camera) return Error("no viewport is open");

        String error;
        const Entity entity = FindEntity(arguments, error);

        if (!entity) return Error(error);

        camera->Focus(entity);

        return CommandCameraGet(arguments, call);
    }

    String AgentBridge::CommandCameraPath(const YAML::Node& arguments, const Ref<AgentServer::Call>& call)
    {
        if (const auto error = RequireViewportError(); !error.empty()) return error;

        if (!GetActiveCamera()) return Error("no viewport is open");

        auto path = CreateScope<CameraPath>();

        if (const auto& waypoints = arguments["waypoints"]; waypoints && waypoints.IsSequence())
        {
            for (const auto& waypoint : waypoints)
            {
                try
                {
                    if (waypoint.IsSequence() && waypoint.size() >= 3)
                    {
                        path->Waypoints.push_back(Vector3{
                            waypoint[0].as<float>(), waypoint[1].as<float>(), waypoint[2].as<float>() });
                    }
                    else if (waypoint.IsMap())
                    {
                        path->Waypoints.push_back(Vector3{
                            waypoint["x"].as<float>(), waypoint["y"].as<float>(), waypoint["z"].as<float>() });
                    }
                }
                catch (...) { return Error("a waypoint is not a position"); }
            }
        }

        if (path->Waypoints.size() < 2)
            return Error("a camera path needs at least two waypoints");

        path->HasLookAtTarget = JsonRead::TryGetVector3(arguments, "lookAt", path->LookAtTarget);

        path->Duration = Math::Max(JsonRead::GetFloat(arguments, "durationSeconds", 5.0f), 0.05f);

        const float samplesPerSecond = Math::Max(JsonRead::GetFloat(arguments, "samplesPerSecond", 10.0f), 0.1f);
        path->SampleInterval = 1.0f / samplesPerSecond;

        //<--- The counters are the point of flying a path at all ---<<
        RenderStatistics::SetEnabled(true);

        path->Call = call;

        const auto camera = GetActiveCamera();
        camera->SetPosition(path->Waypoints.front());

        if (path->HasLookAtTarget)
            camera->LookAt(path->LookAtTarget);

        m_CameraPath = Move(path);

        //<--- Answered when the flight ends ---<<
        return String{};
    }

    void AgentBridge::TickCameraPath(const float deltaTime)
    {
        if (!m_CameraPath) return;

        const auto camera = GetActiveCamera();

        if (!camera)
        {
            AgentServer::Complete(m_CameraPath->Call, Error("the viewport was closed while the camera was flying"));
            m_CameraPath.reset();
            return;
        }

        auto& path = *m_CameraPath;

        path.Elapsed += deltaTime;
        path.TimeSinceSample += deltaTime;

        const float progress = Math::Clamp(path.Elapsed / path.Duration, 0.0f, 1.0f);

        // Constant speed along the polyline rather than constant time per leg, so a long leg is not
        // flown faster than a short one and the samples mean the same thing everywhere.
        float totalLength = 0.0f;
        for (size_t i = 1; i < path.Waypoints.size(); ++i)
            totalLength += Vector3::Distance(path.Waypoints[i - 1], path.Waypoints[i]);

        Vector3 position = path.Waypoints.back();

        if (totalLength > 0.0f)
        {
            float travelled = totalLength * progress;

            for (size_t i = 1; i < path.Waypoints.size(); ++i)
            {
                const float legLength = Vector3::Distance(path.Waypoints[i - 1], path.Waypoints[i]);

                if (travelled > legLength && i + 1 < path.Waypoints.size())
                {
                    travelled -= legLength;
                    continue;
                }

                const float t = legLength > 0.0f ? Math::Clamp(travelled / legLength, 0.0f, 1.0f) : 0.0f;
                position = Vector3::Lerp(path.Waypoints[i - 1], path.Waypoints[i], t);
                break;
            }
        }

        camera->SetPosition(position);

        if (path.HasLookAtTarget)
            camera->LookAt(path.LookAtTarget);

        const bool isLastFrame = path.Elapsed >= path.Duration;

        if (path.TimeSinceSample >= path.SampleInterval || isLastFrame || path.Samples.empty())
        {
            path.TimeSinceSample = 0.0f;

            JsonWriter writer;
            writer.BeginObject();
            writer.KeyValue("t", path.Elapsed);
            writer.KeyVector3("position", position);
            WriteStats(writer);

            writer.Key("draws").BeginArray();
            for (const auto& pass : RenderStatistics::GetLastFrame())
            {
                if (pass.DrawCalls == 0 && pass.VertexBuffersCreated == 0) continue;

                writer.BeginObject();
                writer.KeyValue("name", pass.Name);
                writer.KeyValue("drawCalls", pass.DrawCalls);
                writer.KeyValue("triangles", pass.Triangles);
                writer.KeyValue("vertexBuffersCreated", pass.VertexBuffersCreated);
                writer.EndObject();
            }
            writer.EndArray();

            writer.EndObject();

            path.Samples.push_back(writer.Str());
        }

        if (!isLastFrame) return;

        //<--- Every sample is already JSON, so the answer is put together out of them directly ---<<
        String samples;
        for (size_t i = 0; i < path.Samples.size(); ++i)
        {
            if (i > 0) samples += ",";
            samples += path.Samples[i];
        }

        const String response = Format(
            R"({{"ok":true,"durationSeconds":{0},"sampleCount":{1},"samples":[{2}]}})",
            path.Duration, path.Samples.size(), samples);

        AgentServer::Complete(path.Call, response);

        m_CameraPath.reset();
    }

    // ---- screenshot -----------------------------------------------------------------------------

    String AgentBridge::CommandScreenshot(const YAML::Node& arguments, const Ref<AgentServer::Call>& call)
    {
        const auto target = JsonRead::GetString(arguments, "target", "viewport");

        String pathText = JsonRead::GetString(arguments, "path", String{});

        if (pathText.empty())
            pathText = Format("Screenshots/{0}_{1}.png", target, Time::GetFrameNumber());

        const Path savePath = ResolvePath(pathText);

        const int width = JsonRead::GetInt(arguments, "width", 0);
        const int height = JsonRead::GetInt(arguments, "height", 0);

        if (target == "window")
        {
            const auto result = AgentScreenshot::SaveWindow(savePath, width, height);

            if (!result.IsOk) return Error(result.Error);

            JsonWriter writer;
            writer.BeginObject();
            writer.KeyValue("ok", true);
            writer.KeyValue("path", ToString(result.SavedTo));
            writer.KeyValue("width", result.Width);
            writer.KeyValue("height", result.Height);
            writer.EndObject();

            return writer.Str();
        }

        if (target != "viewport" && target != "game")
            return Error("target has to be 'viewport', 'game' or 'window'");

        const bool wantsGame = target == "game";

        if (wantsGame)
        {
            const auto gamePanels = m_Editor.GetPanelManager()->GetPanelsOfType<GamePanel>();

            const bool hasActiveGamePanel = eastl::any_of(gamePanels.begin(), gamePanels.end(),
                [](const Ref<GamePanel>& panel) { return panel->IsActive(); });

            if (!hasActiveGamePanel)
                return Error("no game panel is open");
        }
        else if (const auto error = RequireViewportError(); !error.empty())
        {
            return error;
        }

        auto screenshot = CreateScope<PendingScreenshot>();
        screenshot->WantsGame = wantsGame;
        screenshot->SavePath = savePath;
        screenshot->Width = width;
        screenshot->Height = height;
        screenshot->Call = call;

        m_PendingScreenshot = Move(screenshot);

        //<--- Answered by OnViewportRendered, once there is a frame to save ---<<
        return String{};
    }

    void AgentBridge::TickPendingScreenshot()
    {
        if (!m_PendingScreenshot) return;

        m_PendingScreenshot->FramesWaited++;

        if (m_PendingScreenshot->FramesWaited <= k_MaxScreenshotWaitFrames) return;

        AgentServer::Complete(m_PendingScreenshot->Call, Error("nothing was rendered to capture"));
        m_PendingScreenshot.reset();
    }

    void AgentBridge::OnViewportRendered(const Ref<Texture>& texture)
    {
        if (!m_PendingScreenshot || m_PendingScreenshot->WantsGame) return;

        CapturePendingScreenshot(texture);
    }

    void AgentBridge::OnGameRendered(const Ref<Texture>& texture)
    {
        if (!m_PendingScreenshot || !m_PendingScreenshot->WantsGame) return;

        CapturePendingScreenshot(texture);
    }

    void AgentBridge::CapturePendingScreenshot(const Ref<Texture>& texture)
    {
        const auto result = AgentScreenshot::SaveTexture(texture, m_PendingScreenshot->SavePath,
            m_PendingScreenshot->Width, m_PendingScreenshot->Height);

        if (result.IsOk)
        {
            JsonWriter writer;
            writer.BeginObject();
            writer.KeyValue("ok", true);
            writer.KeyValue("path", ToString(result.SavedTo));
            writer.KeyValue("width", result.Width);
            writer.KeyValue("height", result.Height);
            writer.EndObject();

            AgentServer::Complete(m_PendingScreenshot->Call, writer.Str());
        }
        else
        {
            AgentServer::Complete(m_PendingScreenshot->Call, Error(result.Error));
        }

        m_PendingScreenshot.reset();
    }

    // ---- play mode ------------------------------------------------------------------------------

    String AgentBridge::CommandPlay(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        if (m_Editor.GetSceneState() == SceneState::Play)
            return Ok(String{ R"("sceneState":"play","note":"already playing")" });

        m_Editor.OnScenePlay();

        return Ok(String{ R"("sceneState":"play")" });
    }

    String AgentBridge::CommandStop(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        if (m_Editor.GetSceneState() != SceneState::Play)
            return Ok(String{ R"("sceneState":"edit","note":"not playing")" });

        m_Editor.SetPaused(false);
        m_Editor.OnSceneStop();

        return Ok(String{ R"("sceneState":"edit")" });
    }

    String AgentBridge::CommandPause(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        if (m_Editor.GetSceneState() != SceneState::Play)
            return Error("nothing to pause, the editor is not playing");

        m_Editor.SetPaused(true);

        return Ok(String{ R"("paused":true)" });
    }

    String AgentBridge::CommandResume(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        m_Editor.SetPaused(false);

        return Ok(String{ R"("paused":false)" });
    }

    String AgentBridge::CommandStep(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        if (m_Editor.GetSceneState() != SceneState::Play)
            return Error("stepping only means something in play mode");

        const int frames = Math::Max(JsonRead::GetInt(arguments, "frames", 1), 1);

        m_Editor.SetPaused(true);
        m_Editor.RequestSteps(frames);

        return Ok(Format("\"paused\":true,\"frames\":{0}", frames));
    }

    // ---- scene ----------------------------------------------------------------------------------

    String AgentBridge::CommandSceneOpen(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        const auto pathText = JsonRead::GetString(arguments, "path", String{});
        if (pathText.empty()) return Error("path is required");

        const Path scenePath = ResolvePath(pathText);

        if (!exists(scenePath))
            return Error(Format("{0} does not exist", ToString(scenePath)));

        if (!m_Editor.GetSceneEditor().OpenScene(scenePath))
            return Error(Format("could not open {0}, the log has the reason", ToString(scenePath)));

        return Ok(Format("\"scene\":\"{0}\"", ToString(SceneManager::GetActiveScenePath())));
    }

    String AgentBridge::CommandSceneSave(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        if (!m_Editor.GetSceneEditor().SaveScene())
            return Error("could not save the scene, it may never have been saved to a file");

        return Ok(Format("\"scene\":\"{0}\"", ToString(SceneManager::GetActiveScenePath())));
    }

    String AgentBridge::CommandSceneReload(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        const auto scenePath = SceneManager::GetActiveScenePath();

        if (scenePath.empty()) return Error("the active scene has no file to reload from");

        if (!m_Editor.GetSceneEditor().OpenScene(scenePath))
            return Error("could not reload the scene, the log has the reason");

        return Ok(Format("\"scene\":\"{0}\"", ToString(scenePath)));
    }

    // ---- shaders --------------------------------------------------------------------------------

    String AgentBridge::CommandShaderReload(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        const auto pathText = JsonRead::GetString(arguments, "path", String{});

        ShaderDiagnostics::Clear();

        const auto reloaded = ShaderLibrary::ForceReimport(Path{ pathText.c_str() });

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);

        writer.Key("compiled").BeginArray();
        for (const auto& path : reloaded)
            writer.Value(ToString(path));
        writer.EndArray();

        writer.Key("failed").BeginArray();
        for (const auto& entry : ShaderDiagnostics::GetEntries())
        {
            writer.BeginObject();
            writer.KeyValue("path", entry.Path);
            writer.KeyValue("stage", entry.Stage);
            writer.KeyValue("error", entry.Message);
            writer.EndObject();
        }
        writer.EndArray();

        if (reloaded.empty())
        {
            writer.Key("loadedShaders").BeginArray();
            for (const auto& path : ShaderLibrary::GetLoadedPaths())
                writer.Value(ToString(path));
            writer.EndArray();

            writer.KeyValue("note", "nothing matched that path, loadedShaders lists what the library holds");
        }

        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandShaderValidateLayout(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        using Microsoft::WRL::ComPtr;

        const auto pathText = JsonRead::GetString(arguments, "path", String{});
        if (pathText.empty()) return Error("path is required");

        Path shaderPath{ pathText.c_str() };

        if (!shaderPath.is_absolute())
        {
            //<--- Engine shaders live under the editor's resources, project ones under the project ---<<
            const Path engineRelative = Path{ "LevResources" } / shaderPath;

            if (exists(engineRelative)) shaderPath = engineRelative;
            else if (const auto projectRelative = ResolvePath(pathText); exists(projectRelative)) shaderPath = projectRelative;
        }

        if (!exists(shaderPath))
            return Error(Format("{0} does not exist", ToString(shaderPath)));

        const auto wanted = JsonRead::GetString(arguments, "constantBuffer", String{});

        const std::wstring widePath = shaderPath.wstring();

        // The same entry points the engine compiles with, tried in turn. Reflection only needs one
        // stage that sees the buffer, and a constant buffer is usually in all of them.
        const std::pair<const char*, const char*> stages[]
        {
            { "PSMain", "ps_5_0" },
            { "VSMain", "vs_5_0" },
            { "CSMain", "cs_5_0" },
            { "GSMain", "gs_5_0" },
        };

        ComPtr<ID3DBlob> bytecode;
        String compiledStage;
        String lastError;

        for (const auto& [entryPoint, target] : stages)
        {
            ComPtr<ID3DBlob> blob;
            ComPtr<ID3DBlob> errors;

            const HRESULT result = D3DCompileFromFile(widePath.c_str(), nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, 0, 0, &blob, &errors);

            if (SUCCEEDED(result))
            {
                bytecode = blob;
                compiledStage = target;
                break;
            }

            if (errors)
            {
                String message{ static_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize() };

                //<--- X3501 only means this file has no such stage ---<<
                if (message.find("X3501") == String::npos)
                    lastError = message;
            }
        }

        if (!bytecode)
        {
            return Error(lastError.empty()
                ? String("no VSMain, PSMain, GSMain or CSMain compiled from that file")
                : lastError);
        }

        ComPtr<ID3D11ShaderReflection> reflection;
        if (FAILED(D3DReflect(bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
            IID_ID3D11ShaderReflection, &reflection)))
            return Error("could not reflect the compiled shader");

        D3D11_SHADER_DESC shaderDescription{};
        reflection->GetDesc(&shaderDescription);

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyValue("path", ToString(shaderPath));
        writer.KeyValue("stage", compiledStage);

        writer.Key("constantBuffers").BeginArray();

        bool found = false;

        for (UINT bufferIndex = 0; bufferIndex < shaderDescription.ConstantBuffers; ++bufferIndex)
        {
            ID3D11ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(bufferIndex);

            D3D11_SHADER_BUFFER_DESC bufferDescription{};
            if (FAILED(buffer->GetDesc(&bufferDescription))) continue;

            const String name{ bufferDescription.Name };

            if (!wanted.empty() && name != wanted) continue;

            found = true;

            writer.BeginObject();
            writer.KeyValue("name", name);
            writer.KeyValue("size", static_cast<unsigned>(bufferDescription.Size));

            writer.Key("fields").BeginArray();

            for (UINT variableIndex = 0; variableIndex < bufferDescription.Variables; ++variableIndex)
            {
                ID3D11ShaderReflectionVariable* variable = buffer->GetVariableByIndex(variableIndex);

                D3D11_SHADER_VARIABLE_DESC variableDescription{};
                if (FAILED(variable->GetDesc(&variableDescription))) continue;

                D3D11_SHADER_TYPE_DESC typeDescription{};
                if (ID3D11ShaderReflectionType* type = variable->GetType())
                    type->GetDesc(&typeDescription);

                writer.BeginObject();
                writer.KeyValue("name", String{ variableDescription.Name });
                writer.KeyValue("offset", static_cast<unsigned>(variableDescription.StartOffset));
                writer.KeyValue("size", static_cast<unsigned>(variableDescription.Size));
                writer.KeyValue("type", typeDescription.Name ? String{ typeDescription.Name } : String{});
                writer.KeyValue("used", (variableDescription.uFlags & D3D_SVF_USED) != 0);
                writer.EndObject();
            }

            writer.EndArray();
            writer.EndObject();
        }

        writer.EndArray();

        if (!wanted.empty() && !found)
            writer.KeyValue("note", Format("the shader has no constant buffer called {0}", wanted));

        writer.EndObject();

        return writer.Str();
    }

    // ---- log ------------------------------------------------------------------------------------

    String AgentBridge::CommandLogTail(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        ConsoleLog::ReadItems(m_LogItems, m_LogCursor);

        const int requested = Math::Max(JsonRead::GetInt(arguments, "count", 50), 1);
        const auto contains = JsonRead::GetString(arguments, "contains", String{});
        const auto minimumLevel = JsonRead::GetString(arguments, "level", String{});

        const auto levelOf = [](const String& name) -> int
        {
            if (name == "trace") return spdlog::level::trace;
            if (name == "debug") return spdlog::level::debug;
            if (name == "info") return spdlog::level::info;
            if (name == "warning" || name == "warn") return spdlog::level::warn;
            if (name == "error") return spdlog::level::err;
            if (name == "critical") return spdlog::level::critical;
            return spdlog::level::trace;
        };

        const int threshold = minimumLevel.empty() ? spdlog::level::trace : levelOf(minimumLevel);

        Vector<const ConsoleLog::Item*> matching;

        for (const auto& item : m_LogItems)
        {
            if (static_cast<int>(item.level) < threshold) continue;

            if (!contains.empty() && item.message.find(contains) == String::npos) continue;

            matching.push_back(&item);
        }

        const size_t first = matching.size() > static_cast<size_t>(requested)
            ? matching.size() - static_cast<size_t>(requested)
            : 0;

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyValue("total", static_cast<int>(matching.size()));
        writer.Key("lines").BeginArray();

        for (size_t i = first; i < matching.size(); ++i)
        {
            writer.BeginObject();
            writer.KeyValue("level", String{ spdlog::level::to_string_view(matching[i]->level).data() });
            writer.KeyValue("message", matching[i]->message);
            writer.EndObject();
        }

        writer.EndArray();
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandMissingReferences(const YAML::Node& arguments, const Ref<AgentServer::Call>&)
    {
        //<--- Without a scan this answers with what loading the project and the open scenes ran
        //into, which is the cheap answer. A scan reads every asset on disk ---<<
        if (JsonRead::GetBool(arguments, "scan", false))
            MissingReferences::ScanProject();

        const auto contains = JsonRead::GetString(arguments, "contains", String{});
        const auto references = MissingReferences::GetAll();

        JsonWriter writer;
        writer.BeginObject();
        writer.KeyValue("ok", true);
        writer.KeyValue("total", static_cast<int>(references.size()));
        writer.Key("references").BeginArray();

        int matched = 0;

        for (const auto& reference : references)
        {
            if (!contains.empty()
                && reference.Source.find(contains) == String::npos
                && reference.Location.find(contains) == String::npos)
                continue;

            matched++;

            writer.BeginObject();
            writer.KeyValue("uuid", static_cast<uint64_t>(reference.Reference));
            writer.KeyValue("source", reference.Source);
            writer.KeyValue("location", reference.Location);
            writer.EndObject();
        }

        writer.EndArray();
        writer.KeyValue("matched", matched);
        writer.EndObject();

        return writer.Str();
    }

    String AgentBridge::CommandQuit(const YAML::Node&, const Ref<AgentServer::Call>&)
    {
        // The editor holds the built binaries open, so rebuilding means closing it first. Asking it
        // to close itself is cleaner than killing the process, which skips the shutdown the render
        // device needs.
        App::Get().Close();

        return Ok(String{ R"("closing":true)" });
    }
}
