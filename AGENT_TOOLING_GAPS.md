# Editor tooling gaps for agent work

> **Status: all eight are closed.** The editor now answers commands on `http://127.0.0.1:17890` and
> `tools/mcp/lev-editor-server.mjs` exposes them as MCP tools. See `AGENT_TOOLING.md` for how to
> use them; the rest of this file is kept as the record of what each gap cost.
>
> | # | Gap | Closed by |
> |---|---|---|
> | 1 | Camera | `editor_camera_get/set/focus/path` |
> | 2 | Frame and pass statistics | `editor_stats`, plus `Renderer::GetLastFrameTimings()` for unaveraged per-frame numbers |
> | 3 | Component values at runtime | `component_get/set/add/remove`, `entity_list` |
> | 4 | Viewport capture | `editor_screenshot`, GPU readback, no focus taken |
> | 5 | Play mode | `editor_play/stop/pause/resume/step` |
> | 6 | Entities without hand-written YAML | `entity_create/destroy`, `component_list` |
> | 7 | Draw, triangle and allocation counts | `editor_render_stats`, `Renderer/RenderStatistics` |
> | 8 | Shader reload result | `shader_reload`, `shader_validate_layout` |
>
> Two things found while closing them are worth carrying forward. A shader that failed to hot reload
> used to assert in `Renderer3D::DrawCube` and take the editor down, which is precisely when its
> compiler errors are worth reading — that draw is now skipped instead. And the note in gap 2 about
> per-pass GPU timers being unreliable holds: `editor_render_stats` counts what a pass submitted,
> which is what to reach for when the timers disagree with each other.


Written after implementing planet generation, its atmosphere and two rounds of optimisation on
`feature/planet-generation`. Everything below is something I could not do from outside the editor,
what I did instead, and what it cost. Two of these gaps caused me to report wrong conclusions, which
is the strongest argument for closing them.

Ordered by how much time they cost, not by how hard they look to build.

| # | Gap | Cost | Priority |
|---|---|---|---|
| 1 | Cannot move the editor camera | Wrong diagnosis, twice; ~2 hours | **Highest** |
| 2 | Cannot read frame/pass statistics | Rebuild + run + grep per measurement | **Highest** |
| 3 | Cannot change a component value live | ~25 s + relaunch per tuning experiment | High |
| 4 | Cannot capture the viewport cleanly | Captured the wrong window twice | High |
| 5 | Cannot enter play mode | PhysX collision never exercised at all | Medium |
| 6 | Cannot create entities without hand-written YAML | Silent parse failures | Medium |
| 7 | No per-frame draw/triangle/allocation counts | Guessed at costs repeatedly | Medium |
| 8 | No shader reload result | Uncertain whether a change took effect | Low |

---

## 1. Cannot move the editor camera

**What I couldn't do.** The viewport camera moves only while the right mouse button is held, with
WASD/QE. I cannot hold a mouse button and press keys, so I could never fly the camera: not to the
surface, not around the planet, not along a path.

**What I did instead.** Moved the *planet* in the scene file to fake camera distance, and later added
a temporary synthetic LOD observer that orbited the surface while the real camera stood still.

**What it cost — this is the important part.** You reported an fps drop when moving with WASD. My
synthetic observer moved only the level-of-detail observer, not `params.CameraPosition`, so it could
not trigger the actual cause (the sky cubemap rebuilding on camera movement). I measured a real but
unrelated problem, reported it as the answer, and was wrong. A second time, I "fixed" motion cost by
budgeting mesh uploads and made the tree grow without bound, which I only found by adding more
temporary instrumentation.

**Proposed tools.**

```
editor_camera_get()  -> { position, rotation, fov, near, far, moveSpeed }
editor_camera_set(position?, rotation?, lookAt?, fov?, moveSpeed?)
editor_camera_focus(entityId)          // EditorCamera::Focus already exists, just unexposed
editor_camera_path(waypoints[], durationSeconds, samplesPerSecond)
    -> per-sample { frameMs, gpuMs, passes{}, planetStats{} }
```

`editor_camera_path` is the one that matters. Scripted motion plus per-frame stats is the difference
between guessing at motion cost and measuring it. It would have found the sky-rebuild bug in one run.

---

## 2. Cannot read frame or pass statistics

**What I couldn't do.** The Statistics panel shows exactly what I needed — frame time, GPU time, and
per-pass timings — but I can only see it in a screenshot, and only if the panel happens to be open.

**What I did instead.** Added temporary `Log::CoreTrace` calls into `Renderer::Render`,
`PlanetUpdatePass` and `ShadowMapPass`, rebuilt, ran, `grep`ped the log, then stripped them out again.
I did this four separate times. One strip went wrong and deleted a `for` loop header, which then
failed to compile.

**Proposed tool.**

```
editor_stats()
  -> { fps, frameMs,
       gpu: { totalMs, shadowMapMs, environmentMs, deferredGeometryMs, deferredLightingMs,
              planetSurfaceMs, planetOceanMs, deferredTransparentMs, postProcessingMs,
              particlesMs, debugMs },
       planets: [ { entityId, treeNodes, chunksDrawn, deepestDepth, buildsInFlight,
                    meshUploadsThisFrame, shadowCasters } ] }
```

I added `GetPlanetSurfaceStatistic()` / `GetPlanetOceanStatistic()` to `Renderer` this session, so most
of the plumbing exists. Note the per-pass GPU timers proved **unreliable** when passes are cheap and
adjacent — I watched 10 ms move between the geometry and ocean timers while the total stayed flat.
Worth exposing the total separately and treating the split as a hint.

---

## 3. Cannot change a component value at runtime

**What I couldn't do.** Set a field on a component and see the result. Every tuning experiment meant
editing scene YAML, killing the editor, relaunching, and waiting ~25 s for the project to load.

**Values I wanted to sweep and mostly couldn't:** `ThicknessExaggeration`, `TargetTrianglePixels`,
`ChunkResolution`, `MergeHysteresis`, `MaxMeshUploadsPerFrame`, `DetailStrength`, `SkyIntensity`,
`RenderOcean`, `MaxConcurrentBuilds`.

**What it cost.** I shipped `ThicknessExaggeration = 8` as a default while telling you it was probably
too strong and that 2–3 was my guess — because testing three values would have been three rebuild
cycles. Same for `TargetTrianglePixels`: I left 3 knowing 5–6 was likely better.

**Proposed tools.**

```
entity_list(filter?)                                  -> [ { id, tag, components[] } ]
component_get(entityId, componentKey)                 -> { field: value, ... }
component_set(entityId, componentKey, { field: value })
```

`componentKey` should be the serializer key already used in scene files (`"Planet"`, `"Atmosphere"`,
`"Directional Light"`), so the vocabulary matches what is on disk. Combined with tool 2, a sweep
becomes one call per value instead of one rebuild per value.

---

## 4. Cannot capture the viewport cleanly

**What I couldn't do.** Get an image of the rendered viewport.

**What I did instead.** PowerShell `CopyFromScreen`, which requires the window to be foreground.
`SetForegroundWindow` is unreliable from a background process, so **two captures grabbed the wrong
window** — one caught a YouTube video you were watching, one caught the Claude Code UI. I eventually
found `PrintWindow` with `PW_RENDERFULLCONTENT` works without stealing focus, which is what I used at
the end. Even then I get the whole editor chrome and have to reason about which region is the viewport.

**Proposed tool.**

```
editor_screenshot(target: "viewport" | "window", width?, height?, path?) -> path
```

Viewport-only, at a requested resolution, without touching window z-order or focus. Rendering to an
offscreen target at a fixed size would also make captures comparable between runs, which screen grabs
are not.

---

## 5. Cannot enter play mode

**What I couldn't do.** Press Play. Scene systems only run in play mode.

**What it cost.** `PlanetCollisionSystem` and the PhysX triangle-mesh cooking I added are **completely
unexercised**. The code compiles and is wired in; whether a character actually stands on the terrain is
unknown. Same for `CelestialOrbitSystem` driving the sun — I had to hand-author a sun rotation because
the orbit system does not run while editing.

**Proposed tools.**

```
editor_play() / editor_stop() / editor_pause() / editor_step(frames)
```

`editor_step` matters for determinism: stepping a fixed number of frames makes physics and generation
reproducible in a way that wall-clock running is not.

---

## 6. Cannot create entities without hand-written YAML

**What I couldn't do.** Add an entity with components. I hand-wrote `PlanetTestScene.scene`, guessing
serializer keys from the source (`"Directional Light"`, `"Skybox Renderer"`, `"Celestial Orbit"`).

**What it cost.** A `Color` written as `[1, 1, 1]` instead of four components produced
`yaml-cpp: error at line 31: bad conversion`, which silently dropped the rest of that entity's
components — the scene loaded looking fine and the light was simply missing. Diagnosing that took a
run, a log read and a guess.

**Proposed tools.**

```
entity_create(tag, parentId?, transform?)  -> entityId
component_add(entityId, componentKey)      // defaults from the component's own constructor
component_remove(entityId, componentKey)
scene_open(path) / scene_save(path?) / scene_reload()
```

Constructing components in-engine means defaults and types come from the code rather than from my
guess at the YAML shape.

---

## 7. No per-frame draw call, triangle or allocation counts

**What I couldn't do.** See how many draws a pass issued, how many triangles, or how many GPU buffers
were created that frame.

**What it cost.** I spent a long optimisation round establishing by inference that the planet pass was
submission-bound rather than shader-bound: disabling the per-pixel noise changed nothing, cutting the
biome loop from 16 to 2 changed nothing, halving the chunk count helped a little, and sorting
front-to-back helped a lot. Three of those four experiments were rebuild-and-run cycles that a draw
call counter would have made unnecessary.

**Proposed tool.**

```
editor_render_stats()
  -> { perPass: [ { name, drawCalls, triangles, vertexBuffersCreated, constantBufferUpdates } ] }
```

`vertexBuffersCreated` specifically: mesh creation happens on the main thread and shows up *inside* GPU
timestamps, which is what made the timings so confusing. Counting it separately would have made the
cause obvious.

---

## 8. No result from shader hot reload

**What I couldn't do.** Know whether an edited `.hlsl` reloaded, or failed to compile at runtime.

**What I did instead.** Compiled shaders offline with `fxc` before every run — which is genuinely
useful and I would keep doing it — then watched the log for runtime errors. `fxc` does not catch
mismatches between a shader's constant buffer layout and the C++ struct, which is the failure mode that
actually bites here.

**Proposed tools.**

```
shader_reload(path?)   -> { compiled: [], failed: [ { path, error } ] }
shader_validate_layout(path, constantBufferName) -> { fields: [ { name, offset, size } ] }
```

The second one would let me check `GPUPlanetData` against `PlanetConstantBuffer` mechanically instead
of by eye. I have hand-verified that layout three times this session.

---

## What is currently unverified because of these gaps

Carrying this list forward so it is not lost:

- **PhysX terrain collision** — never run (gap 5).
- **Behaviour while the camera moves** — the WASD fps drop and the detached atmosphere are both
  reported by you and unreproducible by me (gap 1).
- **Whether `ThicknessExaggeration = 8` and `TargetTrianglePixels = 3` are good defaults** — both are
  my untested guesses, and I believe both are too aggressive (gap 3).
- **Biome ground textures** — the `Texture2DArray` path and triplanar sampling have never had an actual
  texture bound to them, only biome tints (gap 6: I could not author a `.biomeset` with textures).
- **The forward rendering technique** — everything was verified in deferred only; the planet and
  atmosphere passes are wired into both.

---

## The loop that would help most

If only one thing gets built, build this:

```
editor_camera_path(waypoints, duration, samplesPerSecond) -> per-sample stats
```

plus `editor_stats()`. Together they turn "does it get slow when you move?" from something only you can
answer into something I can measure, which is where both of my wrong conclusions this session came
from.
