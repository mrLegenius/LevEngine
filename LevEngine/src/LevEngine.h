#pragma once
// /////////////////////////////////////////////////////////
// -- Core -------------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include <yaml-cpp/yaml.h>

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

#include "Renderer/3D/MeshLoading/MeshLoader.h"
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
#include "Renderer/RendererContext.h"
#include "Renderer/RenderCommand.h"

#include "Renderer/BlendState.h"
#include "Renderer/CascadeShadowMap.h"
#include "Renderer/ClearFlags.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/CPUAccess.h"
#include "Renderer/DepthFunc.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Material.h"
#include "Renderer/PipelineState.h"
#include "Renderer/RasterizerState.h"
#include "Renderer/Rect.h"
#include "Renderer/RenderParams.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/RenderTechnique.h"
#include "Renderer/SamplerState.h"
#include "Renderer/Shader.h"
#include "Renderer/StructuredBuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Viewport.h"
#include "Renderer/LightCollection.h"

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
#include "Scene/Systems/Animation/AnimatorInitSystem.h"
#include "Scene/Systems/Animation/AnimatorUpdateSystem.h"
#include "Scene/Systems/Audio/AudioSourceInitSystem.h"
#include "Scene/Systems/Audio/AudioListenerInitSystem.h"

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