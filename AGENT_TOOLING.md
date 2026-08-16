# Driving the editor from outside it

LevEditor listens on `http://127.0.0.1:17890` and answers one JSON command per request. That is the
whole mechanism: everything below is a command, and the MCP bridge in `tools/mcp` turns those
commands into tools an agent can call.

This exists because the editor is driven by a mouse. The camera only moves while the right button is
held, values are typed into an inspector, the statistics panel can only be read by looking at it.
Anything automated used to fake all three — moving the planet instead of the camera, printing timings
into the log and grepping them, editing scene YAML by hand and restarting. `AGENT_TOOLING_GAPS.md`
is the list of what that cost.

## Running it

The server starts with the editor. Nothing to enable.

- `LEV_AGENT_PORT` — a different port. The MCP bridge reads the same variable.
- `LEV_AGENT_DISABLE=1` — no server at all.

It binds to `127.0.0.1` only, so nothing off the machine can reach it.

Check it is up:

```bash
curl -s -X POST http://127.0.0.1:17890/editor_state -d '{}'
```

Every command is `POST /<tool>` with a JSON body, and answers `{"ok":true, ...}` or
`{"ok":false,"error":"..."}`. `POST /help` lists them.

Commands run on the editor's main thread, one per frame, so a command always sees a state the editor
could actually be in. A command that cannot finish in one frame — a camera flight, a screenshot
waiting for the next render — holds the connection open and answers when it is done.

## As MCP tools

`.mcp.json` in the repo root registers `tools/mcp/lev-editor-server.mjs` as an MCP server named
`lev-editor`. It needs node (any version with `fetch`, so 18+) and nothing else — no packages, no
build step. Claude Code picks it up on the next start; other MCP clients want the same command:

```bash
node tools/mcp/lev-editor-server.mjs
```

If the editor is not running, every tool says so rather than failing in a way that reads like the
editor refused.

## The commands

### State and statistics

| Command | What it answers |
|---|---|
| `editor_state` | project, scene, play state, viewport size, selection |
| `editor_stats` | frame time and per pass GPU time of the **last frame**, the one second averages the panel shows, and per planet chunk counters |
| `editor_render_stats` | draw calls, triangles, buffers created and constant buffer updates, per pass |
| `log_tail` | the last log lines, filtered by level or substring |
| `missing_references` | asset references that point at nothing, with the file and the place holding them |

`editor_stats` gives both `gpu` (last frame) and `gpuAverage` (the one second average the Statistics
panel shows). Use the average for a steady reading and the last frame for measuring one moment. Per
pass GPU timings are worth reading and not worth trusting alone: when two adjacent passes are both
cheap the driver moves time between them. `gpu.Frame` is the number that holds still.

`editor_render_stats` turns counting on the first time it is called, so the first answer can be
empty — call it again after a frame. `vertexBuffersCreated` is the field worth watching: mesh upload
happens on the main thread inside a pass, so it lands in that pass' GPU timestamp and makes a cheap
pass look expensive. A timer cannot separate those, this counter can. `{"disable": true}` turns
counting back off.

`missing_references` lists asset UUIDs nothing answers to. Called plain it reports what loading the
project and the scenes opened this session ran into; `{"scan": true}` reads every YAML asset on disk
instead, so a reference in a scene nobody opened is found as well. Each entry names the `source`
file, the `location` inside it (`Entity 'Enemy' / MeshRenderer / Mesh`, or `Address 'Dance1'` for
`ResourcesDatabase.asset`) and the missing `uuid`. The same list is in the editor's
**Missing References** panel, which can also drop dead addresses from the resources database.

```bash
curl -X POST http://127.0.0.1:17890/missing_references -d '{"scan": true}'
```

### Camera

```bash
curl -X POST http://127.0.0.1:17890/editor_camera_set \
  -d '{"position": [0, 40, -260], "lookAt": [0, 10, -500]}'
```

`editor_camera_get`, `editor_camera_set` (position / rotation / lookAt / fov / moveSpeed),
`editor_camera_focus` (frames an entity), and:

```bash
curl -X POST http://127.0.0.1:17890/editor_camera_path -d '{
  "waypoints": [[0,10,-400], [0,60,-150], [0,120,-105]],
  "lookAt": [0, 10, -500],
  "durationSeconds": 4,
  "samplesPerSecond": 2
}'
```

`editor_camera_path` flies the real viewport camera at constant speed along the polyline and returns
one sample per interval, each with position, frame time, per pass GPU time, planet counters and draw
calls. This is the one that answers "does it get slow when the camera moves", because moving the real
camera is what triggers the work a moving camera causes — a moved object or a synthetic observer does
not.

Rotations are `[pitch, yaw, roll]` in degrees, the same order the inspector shows. Forward is `-Z`.

The camera commands need an open viewport. Play mode focuses the game panel and leaves the viewport
hidden behind it, so the first call after entering play mode opens the viewport and asks to be called
again.

### Screenshots

```bash
curl -X POST http://127.0.0.1:17890/editor_screenshot \
  -d '{"target": "viewport", "path": "Screenshots/before.png", "width": 640}'
```

`target` is `viewport` (the editor camera), `game` (the scene camera, which is what to use in play
mode) or `window` (the whole editor, chrome and panels included).

The first two read the render texture back off the GPU: no editor chrome, no focus taken, nothing
else can end up in the image. `window` uses `PrintWindow` with `PW_RENDERFULLCONTENT`, which also
works without the window being in front. `width`/`height` box filter the result, which is what makes
two captures comparable between runs — give one and the other follows the aspect ratio.

Relative paths are relative to the project root.

### Play mode

`editor_play`, `editor_stop`, `editor_pause`, `editor_resume`, `editor_step`.

Scene systems and physics only run in play mode. `editor_step {"frames": 5}` runs a fixed number of
frames while paused, which is what makes a measurement repeatable: wall clock time is not the same
twice, a frame count is.

Entities created at runtime are gone after `editor_stop`, which reloads the scene from disk.

### Entities and components

```bash
curl -X POST http://127.0.0.1:17890/component_set -d '{
  "entity": "Planet",
  "component": "Planet",
  "values": {"TargetTrianglePixels": 6, "Lod": {"MergeHysteresis": 0.4}}
}'
```

`entity_list`, `entity_create`, `entity_destroy`, `component_list`, `component_get`, `component_set`,
`component_add`, `component_remove`.

- `entity` takes a UUID or a tag. The answer always carries the UUID back.
- `component` is the key the scene file uses (`"Planet"`, `"Directional Light"`, `"Skybox Renderer"`).
  `component_list` has all of them.
- `component_get` returns exactly what the scene file would hold, so `component_set` can be given the
  same shape back.
- Only the fields given change; nested maps are merged rather than replaced.
- Values go through the component's own serializer, so whatever the component does with a change — a
  planet rebuilding what the change invalidated, a light re-applying — happens too.
- `component_add` builds the component with its own constructor, so defaults and types come from the
  code rather than from a guess at the YAML.

`Transform` is handled separately (`position`, `rotation`, `scale`, `worldPosition`,
`worldRotation`), because every entity has one and the scene file writes it inline.

### Scenes and shaders

`scene_open`, `scene_save`, `scene_reload`.

```bash
curl -X POST http://127.0.0.1:17890/shader_reload -d '{"path": "Environment/AtmosphereSky.hlsl"}'
```

`shader_reload` recompiles whether or not the file changed and answers with `compiled` and `failed`,
the latter carrying the compiler's own error text. Without a path it does every shader the library
holds, and lists them if nothing matched.

```bash
curl -X POST http://127.0.0.1:17890/shader_validate_layout \
  -d '{"path": "Shaders/Environment/PlanetAtmosphere.hlsl", "constantBuffer": "AtmosphereConstantBuffer"}'
```

`shader_validate_layout` compiles the file and reflects its constant buffers: every field with its
offset and size. That is the check `fxc` cannot do — a shader's cbuffer layout drifting from the C++
struct compiles cleanly and renders garbage.

### editor_quit

The editor holds `LevEngine.dll` and `LevEditor.exe` open, so it has to close before the solution can
be rebuilt. `editor_quit` closes it the way the menu does, which is not the same as killing the
process: the render device gets its shutdown.

## What holds this up

Engine side, all of it usable without the editor:

- `Renderer/RenderStatistics` — per pass draw, triangle and buffer counters, off until asked for.
- `Renderer::GetLastFrameTimings()` — the per pass GPU times of the last frame, unaveraged.
- `Renderer/Shader/ShaderDiagnostics` — compile errors kept rather than only logged.
- `ShaderLibrary::ForceReimport` — recompiles regardless of file times.
- `IComponentSerializer::GetComponentKey/HasComponent/AddComponent/RemoveComponent` — the serializer
  already knows a component's name and how to build one, which is all that is needed to work with a
  component without naming its type at compile time.

Editor side, `LevEditor/src/Agent`:

- `AgentServer` — the loopback HTTP server and the main thread queue.
- `AgentBridge` — the commands, and the ones that span frames.
- `AgentScreenshot` — GPU readback and PNG encoding through WIC.
- `AgentJson` — writes JSON. Reading needs no code: yaml-cpp already parses it.

## Known limits

- One command per frame. Nothing pipelines.
- Per pass GPU timings move between adjacent cheap passes. Trust `gpu.Frame` and the counters.
- `scene_save` saves to the file the scene came from. There is no save-as, because that opens a
  dialog.
- The camera commands drive the viewport camera. The scene camera is a component like any other and
  is moved with `component_set` on its Transform.
