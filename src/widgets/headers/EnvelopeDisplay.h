#pragma once
#include "imgui.h"

void DrawEnvelope(ImVec2 origin, 
                  ImVec2 size, 
                  float& delay, 
                  float& attack, 
                  float& hold, 
                  float& decay, 
                  float& sustain, 
                  float& release,
                  float& viewStart,
                  float& viewEnd);