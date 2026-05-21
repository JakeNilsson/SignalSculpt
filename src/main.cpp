#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_node_editor.h"
#include "imgui-knobs.h"

#include "core/headers/Module.h"
#include "modules/headers/EnvelopeModule.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <vector>
#include <memory>
#include <algorithm>

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* window = SDL_CreateWindow(
        "SignalSculpt",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    SDL_GLContext gl = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(window, gl);
    ImGui_ImplOpenGL3_Init("#version 330");

    // ImGuiStyle& style = ImGui::GetStyle();

    // style.WindowBorderSize = 5;  // may be able to help with style customization? this is kinda what I want + rounded corners but meh

    std::vector<std::unique_ptr<Module>> modules;

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e))  {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginPopupContextVoid("##canvas_context")) {
            ImGui::SeparatorText("MIDI");
            
            ImGui::SeparatorText("Generators");
            /*if (ImGui::MenuItem("Oscillator"))
                modules.push_back(std::make_unique<OscillatorModule>());
            if (ImGui::MenuItem("Wavetable"))
                modules.push_back(std::make_unique<WavetableModule>());*/

            ImGui::SeparatorText("Modulators");
            if (ImGui::MenuItem("Envelope"))
                modules.push_back(std::make_unique<EnvelopeModule>());
            /*if (ImGui::MenuItem("LFO"))
                modules.push_back(std::make_unique<LFOModule>());*/

            ImGui::SeparatorText("Effects");
            /*if (ImGui::MenuItem("Filter"))
                modules.push_back(std::make_unique<FilterModule>());*/

            ImGui::EndPopup();
        }

        for(auto& module : modules) {
            module->draw();
        }

        modules.erase(
            std::remove_if(modules.begin(), modules.end(), 
            [](const std::unique_ptr<Module>& m) { return !m->isOpen(); }),
            modules.end()
        );

        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}