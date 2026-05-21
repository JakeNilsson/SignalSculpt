#pragma once
#include "headers/EnvelopeDisplay.h"
#include "../helpers/DrawDragPoint.h"
#include <cstdio>
#include <imgui_internal.h>

void DrawEnvelope(ImVec2 origin, 
                  ImVec2 size, 
                  float& delay, 
                  float& attack, 
                  float& hold, 
                  float& decay, 
                  float& sustain, 
                  float& release, 
                  float& viewStart, 
                  float& viewEnd) {

    ImGui::SetCursorScreenPos(origin);
    ImGui::Dummy(size); // reserves space without capturing mouse input
    bool areaHovered = ImGui::IsMouseHoveringRect(origin, ImVec2(origin.x + size.x, origin.y + size.y));

    if (areaHovered) {
        float wheel = ImGui::GetIO().MouseWheel;

        if (ImGui::GetIO().KeyCtrl && wheel != 0.0f) {
            float range = viewEnd - viewStart;
            range *= (wheel > 0) ? 0.85f : 1.15f;
            range  = ImClamp(range, 100.0f, 30000.0f);
            viewEnd = viewStart + range; // viewStart never changes — left edge is anchored
        } else if (wheel <= 0.0f || (wheel >= 0.0f && viewStart != 0.0f)) {
            // Scroll left/right
            float shift = (viewEnd - viewStart) * 0.15f * -wheel;
            viewStart = ImMax(0.0f, viewStart + shift);
            viewEnd   = viewEnd + shift;
        }
    }

    ImDrawList* draw = ImGui::GetWindowDrawList();
    
    auto msToX = [&](float ms) {
        return origin.x + ((ms - viewStart) / (viewEnd - viewStart)) * size.x;
    };

    // Convert screen x back to ms (for drag handling)
    auto xToMs = [&](float x) {
        return viewStart + ((x - origin.x) / size.x) * (viewEnd - viewStart);
    };

    // Absolute ms positions of each point
    float t0  = 0.0f;
    float t0a = delay   * 1000.0f;
    float t1  = t0a + attack  * 1000.0f;
    float t1a = t1  + hold    * 1000.0f;
    float t2  = t1a + decay   * 1000.0f;
    float t3  = t2  + release * 1000.0f;

    ImVec2 p0  = { msToX(t0),  origin.y + size.y };
    ImVec2 p0a = { msToX(t0a), origin.y + size.y };
    ImVec2 p1  = { msToX(t1),  origin.y };
    ImVec2 p1a = { msToX(t1a), origin.y };
    ImVec2 p2  = { msToX(t2),  origin.y + (1.0f - sustain) * size.y };
    ImVec2 p3  = { msToX(t3),  origin.y + size.y };

    // Draw grid lines with ms labels
    // Calculate a nice grid step based on current view width
    auto niceGridStep = [&]() -> float {
        float viewRange  = viewEnd - viewStart;
        float rawStep    = viewRange / 10.0f; // target ~6 grid lines visible at once

        // Round to nearest "nice" value: 50, 100, 250, 500, 1000, 2500, 5000ms etc.
        float niceSteps[] = { 25, 50, 100, 250, 500, 1000, 2500, 5000, 10000 };
        float best = niceSteps[0];
        for (float s : niceSteps)
            if (s >= rawStep) { best = s; break; }
        return best;
    };

    float gridStep = niceGridStep();
    float stepPixels = (gridStep / (viewEnd - viewStart)) * size.x;

    for (float ms = 0; ms <= viewEnd + gridStep; ms += gridStep) {
        float x = msToX(ms);
        if (x < origin.x || x > origin.x + size.x) continue;

        draw->AddLine({x, origin.y}, {x, origin.y + size.y},
                    IM_COL32(255, 255, 255, 20), 0.5f);

        // Only draw label if there's enough room
        // ~40px minimum between labels to avoid overlap
        if (stepPixels > 40.0f) {
            char label[16];
            snprintf(label, sizeof(label), "%.0fms", ms);
            draw->AddText({x + 3, origin.y + 4},
                        IM_COL32(255, 255, 255, 60), label);
        }
    }

    if(delay != 0) {
        draw -> AddLine(p0, p0a, IM_COL32(255,160,40,255), 2.0f);
        draw -> AddBezierCubic(p0a, p0a, p1, p1, IM_COL32(255,160,40,255), 2.0f);
    } else {
        draw -> AddBezierCubic(p0, p0, p1, p1, IM_COL32(255,160,40,255), 2.0f);
    }

    if(hold != 0) {
        draw -> AddLine(p1, p1a, IM_COL32(255,160,40,255), 2.0f);
        draw -> AddBezierCubic(p1a, p1a, p2, p2, IM_COL32(255,160,40,255), 2.0f);
    } else {
        draw -> AddBezierCubic(p1, p1, p2, p2, IM_COL32(255,160,40,255), 2.0f);
    }

    draw -> AddBezierCubic(p2, p2, p3, p3, IM_COL32(255,160,40,255), 2.0f);

    // Points — placed AFTER curves so they render on top
    // p0a — end of delay (X only)
    float dummy = 0.0f;

    DrawDragPoint(draw, "##pt_delay",
        p0a, true, false,
        delay, dummy,
        0.0f, 4.0f, 0.0f, 0.0f,
        origin, size, viewStart, viewEnd);

    // p1 — end of attack (X only, Y is always top)
    DrawDragPoint(draw, "##pt_attack",
        p1, true, false,
        attack, dummy,
        0.0f, 32.0f, 0.0f, 0.0f,
        origin, size, viewStart, viewEnd);

    // p1 — end of attack (X only, Y is always top)
    DrawDragPoint(draw, "##pt_hold",
        p1a, true, false,
        hold, dummy,
        0.0f, 4.0f, 0.0f, 0.0f,
        origin, size, viewStart, viewEnd);

    // p2 — end of decay (X = decay time, Y = sustain level)
    DrawDragPoint(draw, "##pt_decay",
        p2, true, true,
        decay, sustain,
        0.0f, 32.0f, 0.0f, 1.0f,
        origin, size, viewStart, viewEnd);

    // p3 — end of release (X only)
    DrawDragPoint(draw, "##pt_release",
        p3, true, false,
        release, dummy,
        0.0f, 32.0f, 0.0f, 0.0f,
        origin, size, viewStart, viewEnd);

    ImGui::SetCursorScreenPos(ImVec2(origin.x, origin.y + size.y + 10.0f));
}