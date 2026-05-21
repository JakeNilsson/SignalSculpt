#include "headers/EnvelopeModule.h"
#include "imgui-knobs.h"

void EnvelopeModule::draw() {
    ImGui::Begin(windowTitle().c_str(), &m_open);

    ImVec2 origin = ImGui::GetCursorScreenPos(); // where the cursor is right now in screen space
    ImVec2 size   = ImVec2(ImGui::GetContentRegionAvail().x, 100.0f); // full width, 100px tall
    DrawEnvelope(origin, size, m_delay, m_attack, m_hold, m_decay, m_sustain, m_release, m_viewstart, m_viewend);

    ImGuiKnobs::Knob("Delay", &m_delay, 0.0f, 4.0f, 0.02f, "%.3fs", ImGuiKnobVariant_Tick); ImGui::SameLine();
    ImGuiKnobs::Knob("Attack", &m_attack, 0.0f, 32.0f, 0.16f, "%.3fs", ImGuiKnobVariant_Tick); ImGui::SameLine();
    ImGuiKnobs::Knob("Hold", &m_hold, 0.0f, 4.0f, 0.02f, "%.3fs", ImGuiKnobVariant_Tick); ImGui::SameLine();
    ImGuiKnobs::Knob("Decay", &m_decay, 0.0f, 32.0f, 0.16f, "%.3fs", ImGuiKnobVariant_Tick); ImGui::SameLine();
    ImGuiKnobs::Knob("Sustain", &m_sustain, 0.0f, 1.0f, 0.005f, "%.3fs", ImGuiKnobVariant_Tick); ImGui::SameLine();
    ImGuiKnobs::Knob("Release", &m_release, 0.0f, 32.0f, 0.16f, "%.3fs", ImGuiKnobVariant_Tick);

    ImGui::End();
}

void EnvelopeModule::process() {
    // DSP goes here later
}