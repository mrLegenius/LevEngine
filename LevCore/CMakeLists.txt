cmake_minimum_required(VERSION 3.23)
project(LevEngineCore)

add_compile_definitions(LEV_DEBUG)

add_library(LevEngineCore STATIC

        src/Kernel/Application.cpp
        src/Kernel/Logger.cpp
        src/Kernel/Layer.cpp
        src/Kernel/LayerStack.cpp
        src/Kernel/Time.cpp
        src/Kernel/Timestep.cpp
        src/Kernel/UUID.cpp

        src/Renderer/Buffer.cpp
        src/Renderer/Framebuffer.cpp
        src/Renderer/UniformBuffer.cpp
        src/Renderer/StorageBuffer.cpp
        src/Renderer/RenderCommand.cpp
        src/Renderer/Renderer.cpp
        src/Renderer/RendererAPI.cpp
        src/Renderer/Renderer2D.cpp

        src/Renderer/Shader.cpp
        src/Renderer/Texture.cpp
        src/Renderer/Texture.cpp
        src/Renderer/VertexArray.cpp

        src/Renderer/Camera/Camera.cpp
        src/Renderer/Camera/EditorCamera.cpp
        src/Renderer/Camera/OrthographicCamera.cpp
        src/Renderer/Camera/SceneCamera.cpp

        src/Renderer/3D/Mesh.cpp
        src/Renderer/3D/Renderer3D.cpp

        src/Platforms/OpenGL/OpenGLBuffer.cpp
        src/Platforms/OpenGL/OpenGLContext.cpp
        src/Platforms/OpenGL/OpenGLFramebuffer.cpp
        src/Platforms/OpenGL/OpenGLUniformBuffer.cpp
        src/Platforms/OpenGL/OpenGLStorageBuffer.cpp
        src/Platforms/OpenGL/OpenGLRendererAPI.cpp
        src/Platforms/OpenGL/OpenGLShader.cpp
        src/Platforms/OpenGL/OpenGLTexture.cpp
        src/Platforms/OpenGL/OpenGLTextureSkybox.cpp
        src/Platforms/OpenGL/OpenGLVertexArray.cpp

        src/Platforms/Windows/WindowsWindow.cpp
        src/Platforms/Windows/WindowsInput.cpp
        src/Platforms/Windows/WindowsPlatformUtils.cpp

        src/GUI/ImGuiBuild.cpp
        src/GUI/ImGuiLayer.cpp

        src/Math/Math.cpp

        src/Scene/Entity.cpp
        src/Scene/Scene.cpp
        src/Scene/SceneSerializer.cpp
        src/Scene/Components/CameraControllers/OrthographicCameraController.cpp

        LevEngine.h
        )

target_precompile_headers(LevEngineCore PUBLIC pch.h)

add_subdirectory(external/GLFW)
add_subdirectory(external/spdlog)
add_subdirectory(external/glad)
add_subdirectory(external/glm)
add_subdirectory(external/stb)
add_subdirectory(external/imgui)
add_subdirectory(external/entt)
add_subdirectory(external/yaml-cpp)
add_subdirectory(external/ImGuizmo)

target_link_libraries(LevEngineCore spdlog::spdlog_header_only)
target_link_libraries(LevEngineCore glfw)
target_link_libraries(LevEngineCore glad)
target_link_libraries(LevEngineCore glm::glm)
target_link_libraries(LevEngineCore stb)
target_link_libraries(LevEngineCore imgui)
target_link_libraries(LevEngineCore entt)
target_link_libraries(LevEngineCore yaml-cpp)
target_link_libraries(LevEngineCore ImGuizmo)

target_include_directories(LevEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(LevEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/glm/glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/stb/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/entt/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/yaml-cpp/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/ImGuizmo)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

