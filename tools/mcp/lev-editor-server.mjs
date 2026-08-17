#!/usr/bin/env node
// MCP bridge to a running LevEditor.
//
// The editor listens on 127.0.0.1 (see LevEditor/src/Agent) and answers one JSON command per POST.
// This turns those commands into MCP tools. It holds no state: if the editor is not running, every
// call says so instead of failing in a way that looks like the editor said no.

import process from 'node:process';

const PORT = process.env.LEV_AGENT_PORT || '17890';
const HOST = process.env.LEV_AGENT_HOST || '127.0.0.1';
const BASE_URL = `http://${HOST}:${PORT}`;

// A camera path can run for a minute, and the editor blocks while a scene loads.
const REQUEST_TIMEOUT_MS = 310_000;

const ENTITY_REF = {
  entity: {
    type: 'string',
    description: "The entity's UUID as a string, or its tag. entity_list returns both.",
  },
};

const TOOLS = [
  {
    name: 'editor_state',
    description:
      'What the editor currently has open: project, scene, play state, viewport size and selection. ' +
      'Call this first to check the editor is up.',
    inputSchema: { type: 'object', properties: {} },
  },
  {
    name: 'editor_stats',
    description:
      'Frame time and GPU time of the LAST frame (not averaged), the one-second averages the ' +
      'statistics panel shows, and per planet chunk counters. Per pass GPU timings are a hint: when ' +
      'two adjacent passes are both cheap the driver moves time between them. The total holds still.',
    inputSchema: { type: 'object', properties: {} },
  },
  {
    name: 'editor_render_stats',
    description:
      'Draw calls, triangles, vertex/index buffers created and constant buffer updates, per pass, ' +
      'for the last frame. Counting is off until this is called once, so the first answer may be ' +
      'empty - call it again after a frame. vertexBuffersCreated is the one to watch: mesh upload ' +
      'happens inside a pass and lands in that pass GPU timing.',
    inputSchema: {
      type: 'object',
      properties: {
        disable: { type: 'boolean', description: 'Turns counting back off.' },
      },
    },
  },
  {
    name: 'editor_camera_get',
    description: 'Position, rotation, field of view and move speed of the viewport camera.',
    inputSchema: { type: 'object', properties: {} },
  },
  {
    name: 'editor_camera_set',
    description:
      'Moves the viewport camera. Any subset of the fields; lookAt is applied after position, so ' +
      'one call can place the camera and aim it.',
    inputSchema: {
      type: 'object',
      properties: {
        position: { type: 'array', items: { type: 'number' }, description: '[x, y, z] world position.' },
        rotation: {
          type: 'array',
          items: { type: 'number' },
          description: '[pitch, yaw, roll] in degrees.',
        },
        lookAt: { type: 'array', items: { type: 'number' }, description: '[x, y, z] to aim at, keeping the horizon level.' },
        fov: { type: 'number', description: 'Vertical field of view in degrees.' },
        moveSpeed: { type: 'number', description: 'Units per second the camera flies at, 0.5 to 100.' },
      },
    },
  },
  {
    name: 'editor_camera_focus',
    description: 'Frames an entity and its children in the viewport, keeping the current orientation.',
    inputSchema: { type: 'object', properties: { ...ENTITY_REF }, required: ['entity'] },
  },
  {
    name: 'editor_camera_path',
    description:
      'Flies the camera through waypoints at constant speed and returns per sample statistics - ' +
      'frame time, GPU time per pass, planet counters and draw calls. This is how to answer "does it ' +
      'get slow when the camera moves": moving the real camera is what triggers the work a moving ' +
      'camera causes, which a moved object or a synthetic observer does not.',
    inputSchema: {
      type: 'object',
      properties: {
        waypoints: {
          type: 'array',
          items: { type: 'array', items: { type: 'number' } },
          description: 'At least two [x, y, z] world positions.',
        },
        lookAt: {
          type: 'array',
          items: { type: 'number' },
          description: '[x, y, z] the camera keeps aimed at for the whole flight. Optional.',
        },
        durationSeconds: { type: 'number', description: 'How long the flight takes. Default 5.' },
        samplesPerSecond: { type: 'number', description: 'How often to sample. Default 10.' },
      },
      required: ['waypoints'],
    },
  },
  {
    name: 'editor_screenshot',
    description:
      'Saves a PNG of the rendered viewport (no editor chrome, no focus stealing, nothing else can ' +
      'end up in the image) or of the whole editor window. width/height box filter the result, which ' +
      'is what makes two captures comparable between runs.',
    inputSchema: {
      type: 'object',
      properties: {
        target: {
          type: 'string',
          enum: ['viewport', 'game', 'window'],
          description:
            "Default viewport (the editor camera). 'game' is what the scene camera sees, which is " +
            'what to use in play mode. Only viewport and game leave out the editor chrome.',
        },
        path: { type: 'string', description: 'Where to save it. Relative paths are relative to the project root.' },
        width: { type: 'number' },
        height: { type: 'number' },
      },
    },
  },
  {
    name: 'editor_play',
    description: 'Enters play mode, which is the only state where scene systems and physics run.',
    inputSchema: { type: 'object', properties: {} },
  },
  { name: 'editor_stop', description: 'Leaves play mode and reloads the scene from disk.', inputSchema: { type: 'object', properties: {} } },
  { name: 'editor_pause', description: 'Stops running systems while staying in play mode.', inputSchema: { type: 'object', properties: {} } },
  { name: 'editor_resume', description: 'Runs systems again after a pause.', inputSchema: { type: 'object', properties: {} } },
  {
    name: 'editor_step',
    description:
      'Runs a fixed number of frames while paused. A frame count is reproducible in a way wall clock ' +
      'time is not, so this is how to make physics and generation repeat exactly.',
    inputSchema: {
      type: 'object',
      properties: { frames: { type: 'number', description: 'Default 1.' } },
    },
  },
  {
    name: 'entity_list',
    description: 'Entities in the active scene with their ids, tags, positions and component keys.',
    inputSchema: {
      type: 'object',
      properties: {
        tag: { type: 'string', description: 'Only entities whose tag contains this.' },
        component: { type: 'string', description: 'Only entities that have this component.' },
        limit: { type: 'number', description: 'Default 500.' },
        components: { type: 'boolean', description: 'Include the component list of each entity. Default true.' },
      },
    },
  },
  {
    name: 'entity_create',
    description:
      'Creates an entity, optionally with a parent, a transform and components. Components are built ' +
      'by their own constructors, so the defaults and the types come from the code rather than from a ' +
      'guess at what the YAML looks like.',
    inputSchema: {
      type: 'object',
      properties: {
        tag: { type: 'string', description: 'The name shown in the hierarchy.' },
        parent: { type: 'string', description: 'UUID or tag of the parent entity.' },
        position: { type: 'array', items: { type: 'number' } },
        rotation: { type: 'array', items: { type: 'number' }, description: '[pitch, yaw, roll] degrees.' },
        scale: { type: 'array', items: { type: 'number' } },
        components: { type: 'array', items: { type: 'string' }, description: 'Component keys to add, see component_list.' },
      },
    },
  },
  {
    name: 'entity_destroy',
    description: 'Destroys an entity and its children.',
    inputSchema: { type: 'object', properties: { ...ENTITY_REF }, required: ['entity'] },
  },
  {
    name: 'component_list',
    description: 'Every component key the engine can serialize. These are the same names scene files use.',
    inputSchema: { type: 'object', properties: {} },
  },
  {
    name: 'component_get',
    description: "Reads a component's fields, exactly as they would be written to a scene file.",
    inputSchema: {
      type: 'object',
      properties: { ...ENTITY_REF, component: { type: 'string', description: 'Component key, e.g. "Planet".' } },
      required: ['entity', 'component'],
    },
  },
  {
    name: 'component_set',
    description:
      'Writes some of a component\'s fields at runtime, no rebuild and no restart. Only the fields ' +
      'given change; the rest keep what they have. The values go through the component\'s own ' +
      'deserializer, so whatever it does with a change (a planet rebuilding, a light re-applying) ' +
      'happens too. Nested fields are merged, so {"Lod": {"TargetTrianglePixels": 5}} leaves the ' +
      'rest of Lod alone.',
    inputSchema: {
      type: 'object',
      properties: {
        ...ENTITY_REF,
        component: { type: 'string' },
        values: { type: 'object', description: 'The fields to change, in the shape component_get returns.' },
      },
      required: ['entity', 'component', 'values'],
    },
  },
  {
    name: 'component_add',
    description: 'Adds a default constructed component, and sets values in the same call if given.',
    inputSchema: {
      type: 'object',
      properties: { ...ENTITY_REF, component: { type: 'string' }, values: { type: 'object' } },
      required: ['entity', 'component'],
    },
  },
  {
    name: 'component_remove',
    description: 'Removes a component from an entity.',
    inputSchema: {
      type: 'object',
      properties: { ...ENTITY_REF, component: { type: 'string' } },
      required: ['entity', 'component'],
    },
  },
  {
    name: 'editor_undo',
    description:
      'Takes back the last change to the scene, whoever made it -- an inspector edit, a gizmo drag, ' +
      'an entity created or destroyed, or a component_set of your own. Answers with what was undone.',
    inputSchema: {
      type: 'object',
      properties: { steps: { type: 'number', description: 'How many steps to take back. Default 1.' } },
    },
  },
  {
    name: 'editor_redo',
    description: 'Does the last undone change again.',
    inputSchema: {
      type: 'object',
      properties: { steps: { type: 'number', description: 'How many steps to redo. Default 1.' } },
    },
  },
  {
    name: 'scene_open',
    description: 'Opens a scene file. Relative paths are relative to the project root.',
    inputSchema: { type: 'object', properties: { path: { type: 'string' } }, required: ['path'] },
  },
  { name: 'scene_save', description: 'Saves the active scene to the file it came from.', inputSchema: { type: 'object', properties: {} } },
  { name: 'scene_reload', description: 'Reloads the active scene from disk, dropping runtime changes.', inputSchema: { type: 'object', properties: {} } },
  {
    name: 'shader_reload',
    description:
      'Recompiles shaders whether or not the file changed and reports which ones failed with the ' +
      'compiler error. Without a path, every shader the library holds.',
    inputSchema: {
      type: 'object',
      properties: {
        path: { type: 'string', description: 'A path or a suffix of one, e.g. "Environment/AtmosphereSky.hlsl".' },
      },
    },
  },
  {
    name: 'shader_validate_layout',
    description:
      "Compiles a .hlsl and reflects its constant buffers: every field with its offset and size. " +
      'This is what checks a shader\'s cbuffer against the C++ struct mechanically instead of by eye - ' +
      'fxc cannot see that mismatch and it is the one that actually bites.',
    inputSchema: {
      type: 'object',
      properties: {
        path: { type: 'string', description: 'Path to the .hlsl file.' },
        constantBuffer: { type: 'string', description: 'Only this buffer. Omit for all of them.' },
      },
      required: ['path'],
    },
  },
  {
    name: 'editor_quit',
    description:
      'Closes the editor. It holds the built binaries open, so this is what to call before ' +
      'rebuilding the solution.',
    inputSchema: { type: 'object', properties: {} },
  },
  {
    name: 'log_tail',
    description: 'The last lines of the editor log, optionally filtered by level or substring.',
    inputSchema: {
      type: 'object',
      properties: {
        count: { type: 'number', description: 'Default 50.' },
        level: { type: 'string', enum: ['trace', 'debug', 'info', 'warning', 'error', 'critical'] },
        contains: { type: 'string' },
      },
    },
  },
  {
    name: 'missing_references',
    description:
      'Asset references that point at nothing, each with the file holding it and the place inside ' +
      'that file (entity and component, or the address in ResourcesDatabase). Without scan it ' +
      'answers with what loading the project and the open scenes ran into; with scan it reads every ' +
      'YAML asset of the project, including the ones nobody opened.',
    inputSchema: {
      type: 'object',
      properties: {
        scan: { type: 'boolean', description: 'Rescan the whole project first. Default false.' },
        contains: { type: 'string', description: 'Only references whose source or location matches.' },
      },
    },
  },
];

async function callEditor(tool, args) {
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), REQUEST_TIMEOUT_MS);

  try {
    const response = await fetch(`${BASE_URL}/${tool}`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(args ?? {}),
      signal: controller.signal,
    });

    return await response.text();
  } catch (error) {
    if (error.name === 'AbortError') {
      return JSON.stringify({ ok: false, error: `the editor did not answer within ${REQUEST_TIMEOUT_MS / 1000}s` });
    }

    return JSON.stringify({
      ok: false,
      error:
        `could not reach the editor on ${BASE_URL} (${error.code || error.message}). ` +
        'Start LevEditor - run it with the working directory set to LevEditor/ - or set LEV_AGENT_PORT ' +
        'if it was told to listen somewhere else.',
    });
  } finally {
    clearTimeout(timeout);
  }
}

function write(message) {
  process.stdout.write(`${JSON.stringify(message)}\n`);
}

async function handle(request) {
  const { id, method, params } = request;

  switch (method) {
    case 'initialize':
      return {
        protocolVersion: params?.protocolVersion || '2024-11-05',
        capabilities: { tools: {} },
        serverInfo: { name: 'lev-editor', version: '1.0.0' },
      };

    case 'tools/list':
      return { tools: TOOLS };

    case 'tools/call': {
      const name = params?.name;

      if (!TOOLS.some((tool) => tool.name === name)) {
        return { content: [{ type: 'text', text: `unknown tool ${name}` }], isError: true };
      }

      const text = await callEditor(name, params?.arguments);

      let isError = false;
      try {
        isError = JSON.parse(text).ok === false;
      } catch {
        isError = true;
      }

      return { content: [{ type: 'text', text }], isError };
    }

    case 'ping':
      return {};

    default:
      if (typeof id === 'undefined') return null; // a notification we do not care about

      throw Object.assign(new Error(`unknown method ${method}`), { code: -32601 });
  }
}

let buffer = '';

// A tool call is a network round trip, and the editor can take a while. Closing stdin must not cut
// one short, so the process only leaves once nothing is in flight.
let inFlight = 0;
let stdinClosed = false;

function exitWhenIdle() {
  if (stdinClosed && inFlight === 0) process.exit(0);
}

process.stdin.setEncoding('utf8');
process.stdin.on('data', async (chunk) => {
  buffer += chunk;

  let newline;
  while ((newline = buffer.indexOf('\n')) !== -1) {
    const line = buffer.slice(0, newline).trim();
    buffer = buffer.slice(newline + 1);

    if (!line) continue;

    let request;
    try {
      request = JSON.parse(line);
    } catch {
      continue;
    }

    inFlight++;

    try {
      const result = await handle(request);

      // Notifications have no id and get no answer.
      if (result !== null && typeof request.id !== 'undefined') {
        write({ jsonrpc: '2.0', id: request.id, result });
      }
    } catch (error) {
      if (typeof request.id !== 'undefined') {
        write({
          jsonrpc: '2.0',
          id: request.id,
          error: { code: error.code || -32603, message: error.message },
        });
      }
    } finally {
      inFlight--;
      exitWhenIdle();
    }
  }
});

process.stdin.on('end', () => {
  stdinClosed = true;
  exitWhenIdle();
});
