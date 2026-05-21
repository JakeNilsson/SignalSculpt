#pragma once
#include "../../core/headers/Module.h"
#include "../../widgets/headers/EnvelopeDisplay.h"

class EnvelopeModule : public Module {
    public:
    EnvelopeModule() : Module("Envelope") {}

    void draw() override;
    void process() override;

    private:
    float m_delay   = 0.0f;
    float m_attack  = 0.1f;
    float m_hold    = 0.0f;
    float m_decay   = 0.5f;
    float m_sustain = 0.7f;
    float m_release = 0.3f;

    float m_viewstart = 0.0f;
    float m_viewend   = 4000.0f; // ms
};