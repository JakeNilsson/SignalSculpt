cmake_minimum_required(VERSION 3.10)
project(SignalSculpt)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(SDL)

add_executable(SignalSculpt
    src/main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(SignalSculpt PRIVATE imgui imgui/backends)
target_link_libraries(SignalSculpt PRIVATE SDL2::SDL2 SDL2::SDL2main)

if(WIN32)
    target_link_libraries(SignalSculpt PRIVATE opengl32)
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(SignalSculpt PRIVATE OpenGL::GL)
else()
    target_link_libraries(SignalSculpt PRIVATE GL)
endif()