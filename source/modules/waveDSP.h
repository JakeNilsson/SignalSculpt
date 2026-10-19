#pragma once
#include "moduleDSP.h"
#include <cmath>

class WaveDSP : public ModuleDSP {
public:
    enum Input  { phaseIn, freqIn, mixIn, detIn, voicesIn, numInputPorts };
    enum Output { waveOut, numOutputPorts };

    WaveDSP(const double sampleRate, const int ID) : ModuleDSP(numInputPorts, numOutputPorts) {
        prepare(sampleRate);
        setModuleID(ID);
    }

    void tick() noexcept override {
        phase += freq / static_cast<float> (getSampleRate());
        phase -= std::floor (phase);

        out (waveOut, std::sin (phase * juce::MathConstants<float>::twoPi));
    }

private:
    float phase = 0.f;
    const float freq = 261.63f;
};