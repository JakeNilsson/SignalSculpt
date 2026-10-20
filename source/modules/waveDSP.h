#pragma once

#include <cmath>
#include <atomic>
#include "moduleDSP.h"

class WaveDSP : public ModuleDSP {
public:
    enum Input  { phaseIn, freqIn, mixIn, detIn, voicesIn, numInputPorts };
    enum Output { waveOut, numOutputPorts };

    WaveDSP(const double sampleRate, const int ID) : ModuleDSP(numInputPorts, numOutputPorts) {
        prepare(sampleRate);
        setModuleID(ID);
    }

    void setParameter(int paramID, float value) noexcept override {
        switch (paramID) {
            case phaseIn: {
                phase.store (value, std::memory_order_relaxed);
                break;
            }
            case freqIn: {
                freq.store(value, std::memory_order_relaxed);
                break;
            }
            case mixIn: {
                mix.store(value, std::memory_order_relaxed);
                break;
            }
            default: {
                jassertfalse;
                break;
            }
        }
    }

    void tick() noexcept override {
        constexpr auto twoPi = juce::MathConstants<float>::twoPi;

        phase += twoPi * freq / static_cast<float> (getSampleRate());

        if (phase >= twoPi)
            phase -= twoPi;

        out (waveOut, (mix / 100) * std::sin (phase));
    }

private:
    std::atomic<float> phase { 0.f };
    std::atomic<float> freq { 261.63f };
    std::atomic<float> mix { 80.f };
};