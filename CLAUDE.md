# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

LevEngine is a Windows/DirectX 11 game engine (C++20, MSVC) with an ImGui-based editor. Three first-party
projects in `LevEngine.sln`:

| Project | Type | Notes |
|---|---|---|
| `LevEngine` | **DLL** | the engine (`LevEngine.dll` + import lib `LevEngine.lib`); PCH `levpch.h` |
| `LevEditor` | app | editor built on the engine; owns `LevResources/` (engine shaders, fonts, icons, lua templates) |
| `Sandbox` | app | standalone game sample (`FPSGame`) plus the `Sandbox.levproject` test project |

`DirectXTK` and `FMODStudio` are vendored solution projects. Third-party sources live in
`LevEngine/external/` (entt, EASTL/EABase, imgui, imguizmo, yaml-cpp, spdlog, sol+lua, PhysX,
recastnavigation, assimp, vgjs, fmod, stb, eventhandling). Prebuilt DLLs/LIBs are in `lib/`, copied to
`$(TargetDir)` by a post-build `xcopy`.

## Build & run

Only x64 configurations (`Debug`, `Release`) are real; ARM/Win32 entries in the .sln map to x64.
Output goes to `x64/$(Configuration)/`.

```bash
"C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" LevEngine.sln -p:Configuration=Debug -p:Platform=x64 -m
```

Always build through the **.sln**, never a single `.vcxproj`: library and post-build paths are written
in terms of `$(SolutionDir)`, which MSBuild only sets for a solution build.

All three projects share `x64/$(Configuration)/`, so `LevEngine.dll` already sits next to the two `.exe`s
and no copy step is needed for it.

Running the editor requires the working directory to be `LevEditor/`, because engine resources resolve
relative to CWD (`EngineResourcesRoot = "LevResources"`, see `LevEngine/src/Assets/EngineAssets.h`).
VS debugging does this by default; from a shell:

```bash
cd LevEditor && ../x64/Debug/LevEditor.exe
```

The editor reopens the last project from `LevEditor/SaveData.editor`; `Sandbox/Sandbox.levproject` is the
test project. Project-relative asset paths: `resources/` (source assets + `.meta` files), `AssetsCache/`
(binary cache keyed by UUID), `Settings.yaml` (render + physics settings), `ResourcesDatabase.asset`
(address → UUID map).

There is **no test suite and no linter** in this repo. `LEV_DEBUG` is defined only in Debug and gates
asserts (`LEV_ENABLE_ASSERTS`); profiling is compile-time off (`LEV_PROFILE 0` in `Debugging/Profiler.h`).

### Driving the running editor

A running editor answers JSON commands on `http://127.0.0.1:17890` (`LevEditor/src/Agent`), and
`tools/mcp/lev-editor-server.mjs` exposes them as MCP tools registered in `.mcp.json`. **Reach for
these before adding temporary instrumentation or hand-editing scene YAML** — they cover the camera,
frame and per-pass statistics, draw call counters, component values at runtime, viewport screenshots,
play mode, entity creation, scene loading and shader reload. `POST /help` lists them and
`AGENT_TOOLING.md` explains them.

`editor_quit` closes the editor; it holds the built binaries open, so it has to close before a
rebuild.

### Adding files

Both .vcxproj files list every `ClCompile`/`ClInclude` explicitly — a new `.cpp`/`.h` **must** be added to
the project file or it silently won't compile. New `.cpp` files use the PCH (`levpch.h` for engine,
`pch.h` for editor/sandbox) as the first include.

## Exporting engine API (`LEV_API`)

The engine is a DLL, so anything a client links against has to be exported. `Kernel/Core.h` defines
`LEV_API` as `dllexport` when `LEV_BUILD_DLL` is defined (LevEngine.vcxproj only) and `dllimport`
otherwise. **Every new public class or free function in `LevEngine/src` needs it**, and the header needs
`#include "Kernel/Core.h"`:

```cpp
class LEV_API Foo { ... };          // whole class: members, statics and vtable
LEV_API void Bar();                 // free function
extern LEV_API int g_Baz;           // global
```

Rules of thumb:

- **Do not** mark class templates — they are instantiated in the client, there is nothing to import.
  A member of an explicit *specialization* still needs it (`static LEV_API Node encode(...)` in
  `Scene/Serializers/SerializerUtils.h`).
- Nested types inherit the enclosing class's export under MSVC; enums need nothing.
- Marking a class exports every member, so a member that is *declared but never defined* becomes a link
  error instead of being silently dropped.
- `C4251`/`C4275` (exported class holding non-exported EASTL/STL members) are disabled in all three
  projects — those members are only reachable through exported functions.

Third-party code compiled into the DLL but also called from clients is routed through its own macro,
kept consistent by project defines: `IMGUI_API` (patched into `external/imgui/imconfig.h`, which also
shares `GImGui`, so clients need no `SetCurrentContext`), `EASTL_API`, `YAML_CPP_API`. entt needs
nothing — MSVC builds use `__FUNCSIG__`-based `type_hash`, so component storage keys match across
modules.

`ClassCollection`/`OrderedClassCollection` (`Kernel/ClassCollection.h`) keep their storage in one
exported table (`Detail::GetClassCollectionSlot`, `Kernel/ClassCollection.cpp`) instead of a
function-local static, so a serializer or drawer registered from LevEditor or a game project lands in
the same collection the engine iterates.

## Architecture

### Application & frame loop

`EntryPoint.h` defines `main`, which calls the client-provided `CreateApplication()` (see
`LevEditor/src/LevEditor.cpp`, `Sandbox/src/Sandbox.cpp`). Clients subclass `Application` and
`PushLayer(new SomeLayer)`.

`Application` (`Kernel/Application.cpp`) is a singleton owning the render device, window, `Renderer`,
`Physics`, `ScriptingManager`, `ImGuiLayer` (pushed as overlay) and the vgjs job system. `Run()` schedules
the whole game loop onto vgjs thread 0. Each frame: `Time`/`TimelineRunner` update → window input →
`Layer::OnUpdate` for all layers → `Audio::Update` → `Layer::OnRender` → main render target rebind →
ImGui begin/`Layer::OnGUIRender`/end.

Input is polled state (`Input::IsKeyDown`) fed by window events and reset at end of frame; window events
also flow through `EventDispatcher` down the layer stack (top-first, stops on `e.handled`).

### ECS (entt)

`Scene` wraps an `entt::registry`. `entt` is configured with `ENTT_USE_ATOMIC` and `ENTT_NO_ETO` (set in
both `levpch.h` and `LevEngine.h` — keep them in sync). `Entity` is a thin wrapper over `entt::handle`
with `HasComponent/AddComponent/GetComponent/RemoveComponent`; hierarchy hangs off a root entity created
in the `Scene` constructor.

Systems derive from `System` (`Update(float, entt::registry&)`) and are registered in
`Scene::Initialize()` via `RegisterUpdateSystem<T>()` / `RegisterLateUpdateSystem<T>()` /
`RegisterInitSystem<T>()` / `RegisterGUIRenderSystem<T>()` / `RegisterOneFrame<T>()`. Components can opt
into lifecycle hooks by declaring static `OnConstruct(Entity)` / `OnDestroy(Entity)` and being registered
with `RegisterComponentOnConstruct<T>()` / `RegisterComponentOnDestroy<T>()` — the hook is detected with a
`requires` expression in `Scene.inl`.

`Scene::OnUpdate/OnPhysics/OnLateUpdate/OnRender` dispatch through vgjs. `Scene.cpp` has
`constexpr bool k_IsMultiThreading = false`: multithreaded system dispatch exists but is off. Registry
access from multiple threads is guarded by `EnttMutex::Mutex` in places — keep that convention if you
touch scene iteration.

`SceneManager` is the static owner of the active scene (`GetActiveScene()`, `RequestSceneLoad()` +
`TryLoadRequestedScene()` deferring the actual load to a safe point, `SceneLoaded` event).

### Component triple: component + serializer + drawer

A component is defined in `LevEngine/src/**/Components/`; the same `.cpp` also defines its YAML serializer
as `class FooSerializer final : public ComponentSerializer<Foo, FooSerializer>` overriding
`GetKey/SerializeData/DeserializeData`. Registration is automatic: `ComponentSerializer` holds a static
`ClassRegister<IComponentSerializer, TSerializer>`, so `SceneSerializer` just iterates
`ClassCollection<IComponentSerializer>` (`Kernel/ClassCollection.h`). Components also declare
`REGISTER_PARSE_TYPE(Foo)` (`Scene/Components/TypeParseTraits.h`) to get a string name.

The editor's inspector row lives in `LevEditor/src/GUI/Drawers/Components/FooDrawer.cpp` as
`ComponentDrawer<Foo, FooDrawer, Order>` (self-registering via `OrderedClassRegister`, `Order` controls
inspector ordering, `GetLabel()` uses `/` to nest the Add-Component menu). Asset inspectors follow the
same pattern under `GUI/Drawers/Assets/`, viewport gizmo overlays under `ComponentDebugRenderers/`.

So: adding a component = component struct + `REGISTER_PARSE_TYPE` + serializer in the .cpp + drawer in the
editor + (optionally) Lua binding + registration in `Scene::Initialize` if it needs hooks/systems + add
files to both .vcxproj.

### Renderer

`Renderer` (`Renderer/Renderer.cpp`) builds textures, render targets, `PipelineState`s and two
`RenderTechnique`s (Forward, Deferred; ForwardPlus is `LEV_NOT_IMPLEMENTED`). A technique is an ordered
list of `RenderPass`es (`Begin/Process/End`, skippable via `SetEnabled`); GPU timings come from
`BeginQueryPass`/`EndQueryPass` pairs sampled one frame late into `Statistic`s shown by the editor's
statistics panel. `RenderSettings::RenderTechnique` picks the active technique at runtime.

Rendering API abstraction: interfaces in `Renderer/` (`Texture`, `Shader`, `RenderTarget`,
`ConstantBuffer`, `StructuredBuffer`, `PipelineState`, `Query`, …) with `Create*` factories dispatching on
`RenderSettings::RendererAPI`; the only implementation is `Platform/D3D11/`. Multithreaded command
recording uses `D3D11DeferredContexts` (command lists updated/executed at the top of `Renderer::Render`).

HLSL shaders live in `LevEditor/LevResources/Shaders/` and are accessed through `ShaderAssets::*`
(`Assets/EngineAssets.h`) / `ShaderLibrary`. Both assets and shaders hot-reload: `EditorLayer::OnUpdate`
calls `AssetDatabase::ReimportChangedAssets()` and `ShaderLibrary::ReimportChangedAssets()` every frame,
comparing file write times.

### Assets

`Asset` is the base for every asset type (`Assets/*Asset.h`): YAML `SerializeData/DeserializeData`, plus
optional `.meta` (`SerializeMeta/DeserializeMeta`) and binary cache (`LoadFromCache/SaveToCache` via
`BinaryIO::BinaryReader/BinaryWriter`). `AssetDatabase` is the static registry (UUID and path indexed,
`GetAsset<T>(uuid|path)`, `CreateNewAsset<T>`, import/reimport, `IsAsset*` type probes by extension).
`ResourceManager` adds string "addresses" for runtime loading (`LoadAsset<T>("address")`) backed by
`ResourcesDatabase.asset`, built by `Project::Build()`.

`Project` (static, one loaded at a time) holds root path, start scene, and render/physics settings
serializers.

### Scripting, physics, audio, AI

- **Lua** via sol2. `ScriptingManager` owns the `sol::state`, loads `ScriptAsset`s, registers script-based
  systems into a `Scene` and initializes `ScriptsContainer` components. Engine types are exposed in
  `LuaComponentsBinder.cpp` (`new_usertype<...>`) and `MathLuaBindings.cpp`; script templates live in
  `LevEditor/LevResources/Scripts/`.
- **Physics**: PhysX 5 wrapped by `Physics` (owned by `Application`), driven by `Scene::OnPhysics`;
  `Rigidbody`/`CharacterController` components with PhysX-side event callbacks in `Physics/Events/`.
- **Audio**: FMOD Studio, static `Audio` facade + `AudioPlayer`, `AudioSource`/`AudioListener` components.
- **AI**: recast/detour navmesh (`AI/`, `Scene/Components/NavMesh/`).

## Conventions

- `namespace LevEngine`; editor code in `LevEngine::Editor`; members `m_`, statics `s_`, file-local
  constants `k_`.
- **EASTL, not std**, for engine containers and pointers: `String`, `Vector`, `Map`, `UnorderedMap`, `Set`,
  `Queue`, `Pair`, `Array` (`DataTypes/`), `Ref`/`Scope`/`Weak` = `eastl::shared_ptr`/`unique_ptr`/
  `weak_ptr` with `CreateRef`/`CreateScope`/`CastRef`. `Action<...>`/`Func<...>` for callbacks, `Path` =
  `std::filesystem::path`, `Format(...)` wraps fmt. `std::` shows up only where a dependency forces it
  (e.g. spdlog sinks, `std::mutex`).
- Math is DirectXTK SimpleMath re-exported through `Math/` (`Vector3`, `Matrix`, `Quaternion`, `Color`).
- Macros: `LEV_ASSERT`/`LEV_CORE_ASSERT`, `LEV_THROW`, `LEV_NOT_IMPLEMENTED`, `LEV_PROFILE_FUNCTION`/
  `LEV_PROFILE_SCOPE`, `BIND_EVENT_FN`, `FUNCTION_HANDLER` (eventhandling).
- Logging: `Log::Trace/Info/Warning/Error` for game, `Log::Core*` for engine; the editor console panel
  attaches itself as an spdlog sink.
- Client (non-engine) code includes the umbrella header `LevEngine.h`; engine code includes concrete
  headers and relies on `levpch.h` for externals.
- Template-heavy headers split implementations into `.inl` files included at the bottom of the header.
