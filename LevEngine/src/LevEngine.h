#pragma once
// /////////////////////////////////////////////////////////
// -- Core -------------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#define ENTT_USE_ATOMIC 1 //Enable thread-safe entt
#define ENTT_NO_ETO 1 //Enable empty components (tags) but reduce performance and increase memory consumption

#include <entt/entt.hpp>

#include "eventhandling/eventhandling.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <yaml-cpp/yaml.h>

#include <Recast.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourCrowd.h>

#include <PxPhysicsAPI.h>

#include "fmod_common.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"

// -- Data Types -------------------------------------------

#include "DataTypes/Array.h"
#include "DataTypes/Delegates.h"
#include "DataTypes/Map.h"
#include "DataTypes/Pair.h"
#include "DataTypes/Path.h"
#include "DataTypes/Pointers.h"
#include "DataTypes/Queue.h"
#include "DataTypes/String.h"
#include "DataTypes/UnorderedMap.h"
#include "DataTypes/Vector.h"
#include "DataTypes/Utility.h"

// -- Kernel -------------------------------------------
#include "Kernel/Logger.h"
#include "Kernel/Asserts.h"
#include "Kernel/Application.h"
#include "Kernel/Window.h"
#include "Kernel/Layer.h"

#include "Kernel/PlatformUtils.h"
#include "Kernel/Utils.h"
#include "Kernel/UUID.h"
#include "Kernel/ClassCollection.h"

#include "Renderer/3D/MeshLoading/ModelParser.h"
#include "Renderer/3D/MeshLoading/AnimationLoader.h"
#include "TextureLibrary.h"

#include "Debugging/Profiler.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

#include "Project.h"

// -- GUI --------------------------------------------------

#include "GUI/ImGuiLayer.h"
#include "GUI/GUI.h"

// -- Time -------------------------------------------------

#include "Kernel/Time/Timestep.h"
#include "Kernel/Time/Time.h"
#include "Kernel/Time/Timeline.h"
#include "Kernel/Time/TimelineFactory.h"
#include "Kernel/Time/TimelineRunner.h"

// -- Input ------------------------------------------------

#include "Input/Input.h"
#include "Input/KeyCodes.h"
#include "Input/MouseButtonCodes.h"

// -- ECS --------------------------------------------------

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/System.h"
#include "Scene/SceneManager.h"

#include "Scene/Serializers/SceneSerializer.h"
#include "Scene/Serializers/SerializerUtils.h"

// -- Math -------------------------------------------------

#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Math.h"
#include "Math/Random.h"

// /////////////////////////////////////////////////////////
// -- Renderer ---------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Renderer/Renderer.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderCommand.h"

#include "Renderer/Pipeline/BlendState.h"
#include "Renderer/Lighting//CascadeShadowMap.h"
#include "Renderer/Pipeline/ClearFlags.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/CPUAccess.h"
#include "Renderer/Pipeline/DepthFunc.h"
#include "Renderer/Pipeline/DepthStencilState.h"
#include "Renderer/Pipeline/IndexBuffer.h"
#include "Renderer/Material/Material.h"
#include "Renderer/Pipeline/PipelineState.h"
#include "Renderer/Pipeline/RasterizerState.h"
#include "Renderer/Pipeline/Rect.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Pipeline/RenderTarget.h"
#include "Renderer/RenderTechnique.h"
#include "Renderer/Pipeline/SamplerState.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Pipeline/StructuredBuffer.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Pipeline/VertexBuffer.h"
#include "Renderer/Pipeline/Viewport.h"
#include "Renderer/Lighting/LightCollection.h"

#include "Renderer/Camera/SceneCamera.h"

#include "Renderer/3D/Mesh.h"
#include "Renderer/3D/Primitives.h"

#include "Renderer/DebugRender/DebugRender.h"

// /////////////////////////////////////////////////////////
// -- Components -------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/Emitter/EmitterComponent.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"
#include "Scene/Components/Animation/WaypointMovement.h"
#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/Audio/AudioSource.h"
#include "Scene/Components/Audio/AudioListener.h"

#include "Scene/Components/Components.h"
#include "Scene/Components/ComponentSerializer.h"

// /////////////////////////////////////////////////////////
// -- Systems ----------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Scene/Systems/Animation/WaypointDisplacementByTimeSystem.h"
#include "Scene/Systems/Animation/WaypointPositionUpdateSystem.h"
#include "Scene/Systems/Animation/AnimatorUpdateSystem.h"

// /////////////////////////////////////////////////////////
// -- Physics ----------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Physics/Components/Rigidbody.h"

// /////////////////////////////////////////////////////////
// -- Assets -----------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Assets/AssetDatabase.h"
#include "Assets/Asset.h"
#include "Assets/EngineAssets.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Assets/SkyboxAsset.h"
#include "Assets/AudioBankAsset.h"

#include "ResourceManager.h"

// /////////////////////////////////////////////////////////
// -- Audio -----------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Audio/Audio.h"
#include "Audio/AudioPlayer.h"

