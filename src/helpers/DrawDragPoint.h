#include "../../imgui/imgui.h"
#include <imgui_internal.h>

static bool DrawDragPoint(ImDrawList* draw, const char* id,
                           ImVec2 pos, bool canDragX, bool canDragY,
                           float& paramX, float& paramY,
                           float paramXMin, float paramXMax,
                           float paramYMin, float paramYMax,
                           ImVec2 origin, ImVec2 size,
                           float viewStart, float viewEnd) {

    float radius    = 6.0f;
    float hitRadius = 10.0f; // larger invisible hit area than visual radius

    // Place the invisible button FIRST — input is processed in declaration order
    ImGui::SetCursorScreenPos({pos.x - hitRadius, pos.y - hitRadius});
    ImGui::InvisibleButton(id, {hitRadius * 2.0f, hitRadius * 2.0f});

    bool hovered = ImGui::IsItemHovered();
    bool active  = ImGui::IsItemActive();

    // Draw AFTER so the circle renders on top of the hit area
    ImU32 color = active  ? IM_COL32(255, 255, 255, 255) :  // white when dragging
                  hovered ? IM_COL32(255, 220, 120, 255) :  // lighter when hovered
                            IM_COL32(255, 160, 40,  255);   // orange default

    draw->AddCircleFilled(pos, radius, color);
    draw->AddCircle(pos, radius + 2.0f, IM_COL32(255, 255, 255, 60), 12, 1.0f);

    if (active && ImGui::IsMouseDragging(0)) {
        ImVec2 delta = ImGui::GetMouseDragDelta(0, 0.0f);
        ImGui::ResetMouseDragDelta(0);

        if (canDragX) {
            float msDelta = (delta.x / size.x) * (viewEnd - viewStart);
            paramX += msDelta / 1000.0f;
            paramX  = ImClamp(paramX, paramXMin, paramXMax);
        }
        if (canDragY) {
            paramY -= delta.y / size.y;
            paramY  = ImClamp(paramY, paramYMin, paramYMax);
        }
        return true;
    }
    return false;
}