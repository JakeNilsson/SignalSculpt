cmake_minimum_required(VERSION 3.10)
project(SignalSculpt)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(SDL)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(SignalSculpt
    #sources
    ${SOURCES}

    #imgui
    src/main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.cpp

    # imgui-node-editor
    imgui-node-editor/imgui_node_editor.cpp
    imgui-node-editor/imgui_node_editor_api.cpp
    imgui-node-editor/imgui_canvas.cpp
    imgui-node-editor/crude_json.cpp

    # imgui-knobs
    imgui-knobs/imgui-knobs.cpp
)

target_include_directories(SignalSculpt PRIVATE 
                           imgui 
                           imgui/backends
                           imgui-node-editor
                           imgui-knobs)
target_link_libraries(SignalSculpt PRIVATE SDL2::SDL2 SDL2::SDL2main)

if(WIN32)
    target_link_libraries(SignalSculpt PRIVATE opengl32)
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(SignalSculpt PRIVATE OpenGL::GL)
else()
    target_link_libraries(SignalSculpt PRIVATE GL)
endif()