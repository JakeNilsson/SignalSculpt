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
        smoothDet.reset (getSampleRate(), rampLength);
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
            case detIn: {
                det.store(value, std::memory_order_relaxed);
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
        smoothFreq .setTargetValue (freq .load (std::memory_order_relaxed));
        smoothPhase.setTargetValue (phase.load (std::memory_order_relaxed));
        smoothMix  .setTargetValue (mix  .load (std::memory_order_relaxed));
        smoothDet  .setTargetValue (det  .load (std::memory_order_relaxed));

        const int channelsOut = std::min (getOutputChannels(), getMaxChannels());

        PortFrames output;
        output.numChannels = channelsOut;

        constexpr auto twoPi = juce::MathConstants<float>::twoPi;
        const auto sampRate = getSampleRate();

        const auto baseFreq = smoothFreq.getNextValue();
        const auto baseDet = smoothDet.getNextValue();
        const auto baseMix = smoothMix.getNextValue();
        const auto basePhase = smoothPhase.getNextValue();

        for (int i = 0; i < channelsOut; ++i) {
            float sumL = 0.0f;
            float sumR = 0.0f;

            const auto inShapeType = inMono(shapeTypeIn, i);
            // const auto inShapeSpec = inMono(shapeSpecIn, i);
            const auto inPhase = inMono(phaseIn, i);
            const auto inFreq = inMono(freqIn, i);
            const auto inMix = inMono(mixIn, i);
            const auto inDet = inMono(detIn, i);
            const auto inVoices = inMono(voicesIn, i);

            const auto trueShapeType = std::min(static_cast<int>(std::max(1, static_cast<int>(waveChoice + static_cast<int>(inShapeType * 4)))), 4);
            // const auto trueShapeSpec = std::min(static_cast<int>(std::max(1, static_cast<int>(waveSpec + inShapeSpec))), 1);
            // for truePhase, don't clamp - make sure wrap around is allowed, since that's how the knob is built
            const auto trueFreq = std::clamp (baseFreq + inFreq * 20000.f, -20000.f, 20000.f);
            const auto trueMix = std::min(std::max(0.f, baseMix + (inMix * 100.f)), 100.f);
            const auto trueDet = std::min(std::max(0.f, baseDet + (inDet * 100.f)), 100.f);
            const auto trueVoices = std::min(static_cast<int>(std::max(1, voices + static_cast<int>(inVoices * 16))), 16);

            const float pm = twoPi * (basePhase + inPhase);

            for (int j = 0; j < trueVoices; ++j) {
                const float t = (trueVoices == 1)
                    ? 0.5f
                    : static_cast<float>(j) / static_cast<float>(trueVoices - 1);

                const float position = 2.0f * t - 1.0f;
                const float cents = position * trueDet;

                const float frequency =
                    trueFreq * std::pow(2.0f, cents / 1200.0f);

                float& p = voicePhases[i][j];
                p += twoPi * frequency / static_cast<float> (sampRate);
                p -= twoPi * std::floor (p / twoPi);

                const float s = wave (p + pm, static_cast<WaveType> (trueShapeType));
                sumL += s * t;
                sumR += s * (1 - t);
            }

            const Frame channelOutput = {
                .left = (trueMix / 100) * (sumL / trueVoices),
                .right = (trueMix / 100) * (sumR / trueVoices)
            };

            output.channels[i] = channelOutput;
        }

        out(waveOut, output);
    }

    static float wave(const float wavePhase, const WaveType type) noexcept {
        constexpr float twoPi = 2.0f * juce::MathConstants<float>::pi;
        float cycle = wavePhase / twoPi;
        cycle -= std::floor (cycle);

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
    // std::array<float, 16> voicePhases;
    std::array<std::array<float, 16>, PortFrames::maxChannels> voicePhases {};

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
