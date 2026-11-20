#pragma once

#include "moduleDSP.h"
#include "juce_dsp/juce_dsp.h"

class FilterDSP : public ModuleDSP {
public:
    enum Input  { signalIn, filterTypeIn, filterSpecIn, resIn, cutIn, mixIn, blendIn, numInputPorts };
    enum Output { filterOut, numOutputPorts };

    FilterDSP(const double sampleRate, const int ID) : ModuleDSP(numInputPorts, numOutputPorts) {
        prepare(sampleRate);
        setModuleID(ID);
    }

    void setParameter(const int paramID, const float value) noexcept override {
        switch (paramID) {
            case filterTypeIn: {
                filterType.store(value);
                break;
            } case filterSpecIn: {
                filterSpec.store(value);
                break;
            } case resIn: {
                res.store(value);
                break;
            } case cutIn: {
                cut.store(value);
                break;
            } case mixIn: {
                mix.store(value);
                break;
            } case blendIn: {
                blend.store(value);
                break;
            } default : {
                jassertfalse;
                break;
            }
        }
    }

    void tick() noexcept override {
        const auto sampRate = static_cast<float>(getSampleRate());

        smoothRes.setTargetValue (res.load (std::memory_order_relaxed));
        smoothCut.setTargetValue (cut.load (std::memory_order_relaxed));
        smoothMix.setTargetValue (mix.load (std::memory_order_relaxed));
        smoothBlend.setTargetValue (blend.load (std::memory_order_relaxed));

        const float baseRes = smoothRes.getNextValue();
        const float baseCut = smoothCut.getNextValue();
        const float baseMix = smoothMix.getNextValue();
        const float baseBlend = smoothBlend.getNextValue();

        const int channelsOut = std::min (getOutputChannels(), getMaxChannels());
        PortFrames output;
        output.numChannels = channelsOut;

        const float modOctaves = 0.5f * std::log2 ((0.49f * sampRate) / 8.f);

        for (int i = 0; i < channelsOut; i++) {
            const auto [left, right] = inStereo(signalIn, i);

            const float inMix = inMono(mixIn, i) * 50.f;
            float trueMix = std::clamp((baseMix + inMix), 0.f, 100.f);
            trueMix = trueMix / 100;

            if (!isEnabled || trueMix == 0.f) {
                output.channels[i] = {.left = left, .right = right};
                continue;
            }

            const float inRes = inMono(resIn, i) * (19.5f / 2.f);
            const float inBlend = inMono(blendIn, i);

            const float trueRes = std::clamp((baseRes + inRes), 0.5f, 20.f);
            const float trueCut = std::clamp (baseCut * std::exp2 (inMono (cutIn, i) * modOctaves),8.f, 0.49f * sampRate);
            const float trueBlend = std::clamp((baseBlend + inBlend), 0.f, 1.f);

            if (trueCut != lastCut || trueRes != lastRes) {
                for (auto* f : { &lowBlendFilter, &midBlendFilter, &highBlendFilter }) {
                    f->setCutoffFrequency (trueCut);
                    f->setResonance (trueRes);
                }
                lastCut = trueCut;
                lastRes = trueRes;
            }

            const Frame channelOutput = {
                .left = (((lowBlendFilter.processSample(i * 2, left) * (1 - std::clamp(2 * trueBlend, 0.f, 1.f))) +
                          (midBlendFilter.processSample(i * 2, left) * (1 - std::abs((2 * trueBlend) - 1))) +
                          (highBlendFilter.processSample(i * 2, left) * (std::clamp((2 * trueBlend) - 1, 0.f, 1.f)))) *
                        trueMix) + (left * (1.f - trueMix)),

                .right = (((lowBlendFilter.processSample(i * 2 + 1, right) * (1 - std::clamp(2 * trueBlend, 0.f, 1.f))) +
                           (midBlendFilter.processSample(i * 2 + 1, right) * (1 - std::abs((2 * trueBlend) - 1))) +
                           (highBlendFilter.processSample(i * 2 + 1, right) * (std::clamp((2 * trueBlend) - 1, 0.f, 1.f)))) *
                          trueMix) + (right * (1.f - trueMix))
            };

            output.channels[i] = channelOutput;
        }
        out(filterOut, output);
    }

    void onPrepare() noexcept override {
        const juce::dsp::ProcessSpec spec{.sampleRate = getSampleRate(),
                                          .maximumBlockSize = 1,
                                          .numChannels = static_cast<juce::uint32>(getMaxChannels() * 2)}; // *2 for stereo

        lowBlendFilter.prepare(spec);
        lowBlendFilter.reset();
        lowBlendFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

        midBlendFilter.prepare(spec);
        midBlendFilter.reset();
        midBlendFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);

        highBlendFilter.prepare(spec);
        highBlendFilter.reset();
        highBlendFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);

        smoothRes.reset(getSampleRate(), rampLength);
        smoothCut.reset(getSampleRate(), rampLength);
        smoothMix.reset(getSampleRate(), rampLength);
        smoothBlend.reset (getSampleRate(), rampLength);
    }

private:
    std::atomic<int> filterType { 0 };
    std::atomic<int> filterSpec { 0 };

    std::atomic<float> res { 0.8 };
    juce::LinearSmoothedValue<float> smoothRes { 0.8 };

    std::atomic<float> cut { 425 };
    juce::LinearSmoothedValue<float> smoothCut { 425 };

    std::atomic<float> mix { 100 };
    juce::LinearSmoothedValue<float> smoothMix { 100 };

    std::atomic<float> blend { 0 };
    juce::LinearSmoothedValue<float> smoothBlend { 0 };

    juce::dsp::StateVariableTPTFilter<float> lowBlendFilter;
    juce::dsp::StateVariableTPTFilter<float> midBlendFilter;
    juce::dsp::StateVariableTPTFilter<float> highBlendFilter;

    float lastCut = -1.f;
    float lastRes = -1.f;

    float rampLength = 0.05f;
};
