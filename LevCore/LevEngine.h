#pragma once

// /////////////////////////////////////////////////////////
// -- Core -------------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "src/Kernel/Application.h"
#include "src/Kernel/Layer.h"
#include "src/Kernel/Logger.h"
#include "src/Kernel/Asserts.h"

#include "src/Utils/PlatformUtils.h"
#include "src/Debug/Profiler.h"

// -- GUI --------------------------------------------------

#include "src/GUI/ImGuiLayer.h"

// -- Time -------------------------------------------------

#include "src/Kernel/Timestep.h"
#include "src/Kernel/Time.h"

// -- Input ------------------------------------------------

#include "src/Input/Input.h"
#include "src/Input/KeyCodes.h"
#include "src/Input/MouseButtonCodes.h"

// -- ECS --------------------------------------------------

#include "src/Scene/Scene.h"
#include "src/Scene/Entity.h"
#include "src/Scene/ScriptableEntity.h"
#include "src/Scene/Components.h"

#include "src/Scene/Components/CameraControllers/OrthographicCameraController.h"

#include "src/Scene/SceneSerializer.h"

// -- MATH -------------------------------------------------

#include "src/Math/Math.h"

// /////////////////////////////////////////////////////////
// -- Renderer ---------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

#include "src/Renderer/Renderer.h"
#include "src/Renderer/Renderer2D.h"
#include "src/Renderer/RenderCommand.h"
				 
#include "src/Renderer/Buffer.h"
#include "src/Renderer/VertexArray.h"
#include "src/Renderer/Framebuffer.h"

#include "src/Renderer/Shader.h"
#include "src/Renderer/Texture.h"

#include "src/Renderer/Camera/OrthographicCamera.h"

#include "src/Renderer/3D/Mesh.h"

// /////////////////////////////////////////////////////////
// -- Components -------------------------------------------
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

