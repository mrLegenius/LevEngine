cmake_minimum_required(VERSION 3.23)
project (LevEngine)

add_compile_definitions(LEV_DEBUG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}) #until LevEditor is done

add_executable(${PROJECT_NAME}
        main.cpp
        src/LevEditor.cpp
        src/EditorLayer.cpp
        src/Panels/SceneHierarchy.cpp
        src/Panels/AssetsBrowser.cpp
        )

add_subdirectory(LevCore)
#add_subdirectory(LevEditor)

target_link_libraries(${PROJECT_NAME} LevEngineCore)
#target_link_libraries(${PROJECT_NAME} LevEditor)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/LevCore)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/LevEditor)
