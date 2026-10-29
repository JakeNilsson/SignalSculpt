#pragma once

#include <cmath>
#include <atomic>
#include <algorithm>
#include "moduleDSP.h"
#include <juce_audio_basics/juce_audio_basics.h>

class WaveDSP : public ModuleDSP {
public:
    enum Input  { shapeTypeIn, shapeSpecIn, phaseIn, freqIn, mixIn, detIn, voicesIn, numInputPorts };
    enum Output { waveOut, numOutputPorts };

    enum WaveType { blank, sine, triangle, saw, square };

    WaveDSP(const double sampleRate, const int ID) : ModuleDSP(numInputPorts, numOutputPorts) {
        prepare(sampleRate);
        setModuleID(ID);
    }

    void onPrepare() override {
        smoothPhase.reset(getSampleRate(), rampLength);
        smoothFreq.reset(getSampleRate(), rampLength);
        smoothMix.reset(getSampleRate(), rampLength);
    }

    void setParameter(const int paramID, const float value) noexcept override {
        switch (paramID) {
            case shapeTypeIn: {
                waveChoice.store(static_cast<WaveType>(value), std::memory_order_relaxed);
                shapeChanged = true;
                break;
            }
            case shapeSpecIn: {
                // unimplemented!!!!!!!
                break;
            }
            case phaseIn: {
                phase.store (value, std::memory_order_relaxed);
                smoothPhase.setTargetValue(phase);
                break;
            }
            case freqIn: {
                freq.store(value, std::memory_order_relaxed);
                smoothFreq.setTargetValue(freq);
                break;
            }
            case mixIn: {
                mix.store(value, std::memory_order_relaxed);
                smoothMix.setTargetValue(mix);
                break;
            }
            case detIn: {
                det.store(value, std::memory_order_relaxed);
                smoothDet.setTargetValue(det);
                break;
            }
            case voicesIn: {
                voices.store(static_cast<int>(value), std::memory_order_relaxed);
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
        const auto sampRate = getSampleRate();

        const auto baseFreq = smoothFreq.getNextValue();
        const auto baseDet = smoothDet.getNextValue();
        const auto baseMix = smoothMix.getNextValue();

        float sumL = 0.0f;
        float sumR = 0.0f;

        const auto inShapeType = inMono(shapeTypeIn);
        // const auto inShapeSpec = inMono(shapeSpecIn);
        const auto inPhase = inMono(phaseIn);
        const auto inFreq = inMono(freqIn);
        const auto inMix = inMono(mixIn);
        const auto inDet = inMono(detIn);
        const auto inVoices = inMono(voicesIn);

        const auto trueShapeType = std::min(static_cast<int>(std::max(1, static_cast<int>(waveChoice + static_cast<int>(inShapeType * 4)))), 4);
        // const auto trueShapeSpec = std::min(static_cast<int>(std::max(1, static_cast<int>(waveSpec + inShapeSpec))), 1);
        const auto truePhase = std::min(std::max(0.f, phase + (inPhase * juce::MathConstants<float>::twoPi)), juce::MathConstants<float>::twoPi);
        const auto trueFreq = std::min(std::max(0.f, baseFreq + (inFreq * 20000.f)), 20000.f);
        const auto trueMix = std::min(std::max(0.f, baseMix + (inMix * 100.f)), 100.f);
        const auto trueDet = std::min(std::max(0.f, baseDet + (inDet * 100.f)), 100.f);
        const auto trueVoices = std::min(static_cast<int>(std::max(1, voices + static_cast<int>(inVoices * 16))), 16);

        for (int i = 0; i < trueVoices; ++i) {
            const float t = (trueVoices == 1)
                ? 0.5f
                : static_cast<float>(i) / static_cast<float>(trueVoices - 1);

            const float position = 2.0f * t - 1.0f;
            const float cents = position * trueDet;

            const float frequency =
                trueFreq * std::pow(2.0f, cents / 1200.0f);

            voicePhases[i] += twoPi * frequency / sampRate;

            if (voicePhases[i] >= twoPi)
                voicePhases[i] = std::fmod(voicePhases[i], twoPi);

            sumL += (wave(voicePhases[i], static_cast<WaveType>(trueShapeType)) * t);
            sumR += (wave(voicePhases[i], static_cast<WaveType>(trueShapeType)) * (1 - t));
        }

        const Frame oscillatorOutput = {
            .left = (trueMix / 100) * (sumL / trueVoices),
            .right = (trueMix / 100) * (sumR / trueVoices)
        };

        out(waveOut, oscillatorOutput);
    }

    static float wave(const float wavePhase, const WaveType type) noexcept {
        constexpr float twoPi = 2.0f * juce::MathConstants<float>::pi;
        const float cycle = std::fmod(wavePhase, twoPi) / twoPi;

        switch (type) {
            case blank : {
                return 0.f;
            }
            case sine : {
                return std::sin(wavePhase);
            }
            case triangle : {
                return 1.0f - 4.0f * std::abs(cycle - 0.5f);
            }
            case saw : {
                return 2.0f * cycle - 1.0f;
            }
            case square : {
                return cycle < 0.5f ? 1.0f : -1.0f;
            }
            default : {
                jassertfalse;
                return 0.f;
            }
        }
    }

private:
    std::atomic<WaveType> waveChoice { sine };

    std::atomic<float> phase { 0.f };
    juce::LinearSmoothedValue<float> smoothPhase { 0.f };
    std::array<float, 16> voicePhases;

    std::atomic<float> freq { 261.63f };
    juce::LinearSmoothedValue<float> smoothFreq { 261.63f };

    std::atomic<float> mix { 80.f };
    juce::LinearSmoothedValue<float> smoothMix { 80.f };

    std::atomic<float> det { 20.f };
    juce::LinearSmoothedValue<float> smoothDet { 20.f };

    std::atomic<int> voices { 1 };

    float rampLength = 0.05f;

    bool shapeChanged = false;
};
