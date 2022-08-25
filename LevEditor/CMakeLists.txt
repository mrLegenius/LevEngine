cmake_minimum_required(VERSION 3.23)
project (LevEditor)

add_library(LevEngineEditor
        src/LevEditor.cpp
        src/EditorLayer.cpp
        src/EditorWindows/SceneHierarchyEditorWindow.cpp
        )

target_link_libraries(LevEngineEditor LevEngineCore)

target_include_directories(LevEngineEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(../LevCore)