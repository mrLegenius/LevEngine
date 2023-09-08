#pragma once
// /////////////////////////////////////////////////////////
// -- Core -------------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Kernel/Application.h"
#include "Kernel/Layer.h"
#include "Kernel/Logger.h"

#include "Kernel/Asserts.h"
#include "Kernel/Color.h"
#include "Kernel/PlatformUtils.h"
#include "Kernel/Utils.h"
#include "Kernel/UUID.h"
#include "Kernel/ClassCollection.h"

#include "ObjLoader.h"
#include "TextureLibrary.h"

#include "Debugging/Profiler.h"

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

// -- Data Types -------------------------------------------

#include "DataTypes/Map.h"
#include "DataTypes/Pair.h"
#include "DataTypes/Pointers.h"
#include "DataTypes/Queue.h"
#include "DataTypes/String.h"
#include "DataTypes/UnorderedMap.h"
#include "DataTypes/Vector.h"

// -- GUI --------------------------------------------------

#include "GUI/ImGuiLayer.h"

// -- Time -------------------------------------------------

#include "Kernel/Timestep.h"
#include "Kernel/Time.h"

// -- Input ------------------------------------------------

#include "Input/Input.h"
#include "Input/KeyCodes.h"
#include "Input/MouseButtonCodes.h"

// -- ECS --------------------------------------------------

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/System.h"

#include "Scene/Serializers/SceneSerializer.h"
#include "Scene/Serializers/SerializerUtils.h"

// -- Math -------------------------------------------------

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

#include "Renderer/Camera/SceneCamera.h"

#include "Renderer/3D/Mesh.h"

// /////////////////////////////////////////////////////////
// -- Components -------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/Emitter/EmitterComponent.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"
#include "Scene/Components/SkyboxRenderer/SkyboxRenderer.h"

#include "Scene/Components/Components.h"
#include "Scene/Components/ComponentDrawer.h"
#include "Scene/Components/ComponentSerializer.h"

// /////////////////////////////////////////////////////////
// -- Physics ----------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//#include "Physics/Events/CollisionBeginEvent.h"

// /////////////////////////////////////////////////////////
// -- Assets -----------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "Assets/AssetDatabase.h"
#include "Assets/Asset.h"
#include "Assets.h"
#include "Assets/MaterialAsset.h"
#include "Assets/MeshAsset.h"
#include "Assets/SkyboxAsset.h"