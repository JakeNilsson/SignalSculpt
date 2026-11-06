#pragma once

#include <cmath>
#include <atomic>
#include <algorithm>
#include "moduleDSP.h"
#include "../helpers/DAHDSR.h"
#include <juce_audio_basics/juce_audio_basics.h>

class EnvDSP : public ModuleDSP {
public:
    enum Input  { delayIn, attackIn, holdIn, gateIn, decayIn, sustainIn, releaseIn, numInputPorts };
    enum Output { envOut, numOutputPorts };

    EnvDSP(const double sampleRate, const int ID) : ModuleDSP(numInputPorts, numOutputPorts) {
        prepare(sampleRate);
        setModuleID(ID);
    }

    void onPrepare() override {
        const double sampRate = getSampleRate();
        for (int i = 0; i < PortFrames::maxChannels; i++) {
            envelopes[i].setSampleRate(sampRate);
        }
    };

    void setParameter(const int paramID, const float value) noexcept override {
        switch (paramID) {
            case delayIn: {
                delay.store(value);
                break;
            } case attackIn: {
                attack.store(value);
                break;
            } case holdIn: {
                hold.store(value);
                break;
            } case gateIn: {
                break;
            } case decayIn: {
                decay.store(value);
                break;
            } case sustainIn: {
                sustain.store(value);
                break;
            } case releaseIn: {
                release.store(value);
                break;
            } default : {
                jassertfalse;
                break;
            }
        }
    }

    void tick() noexcept override {
        const int channelsOut = std::min (getOutputChannels(), getMaxChannels());
        PortFrames output;
        output.numChannels = channelsOut;

        for (int i = 0; i < channelsOut; i++) {
            const float gate = inMono(gateIn, i);

            const float inDelay = inMono(delayIn, i);
            const float inHold = inMono(holdIn, i);
            const float inAttack = inMono(attackIn, i);
            const float inDecay = inMono(decayIn, i);
            const float inSustain = inMono(sustainIn, i);
            const float inRelease = inMono(releaseIn, i);

            const float trueDelay = std::clamp((delay + inDelay), 0.f, 4.f);
            const float trueAttack = std::clamp((attack + inAttack), 0.f, 32.f);
            const float trueHold = std::clamp((hold + inHold), 0.f, 4.f);
            const float trueDecay = std::clamp((decay + inDecay), 0.f, 32.f);
            const float trueSustain = std::clamp((sustain + inSustain), 0.f, 1.f);
            const float trueRelease = std::clamp((release + inRelease), 0.f, 32.f);

            if (envelopes[i].DAHDSRChanged(trueDelay, trueAttack, trueHold, trueDecay, trueSustain, trueRelease)) {
                envelopes[i].setDAHDSR(trueDelay, trueAttack, trueHold, trueDecay, trueSustain, trueRelease);
            }

            if (gate > 0.f) {
                envelopes[i].gateOn();
            } else {
                envelopes[i].gateOff();
            }

            const float nextSig = envelopes[i].getNextValue();

            const Frame channelOutput = {
                .left = nextSig,
                .right = nextSig
            };

            output.channels[i] = channelOutput;
        }
        out(envOut, output);
    }

private:

    std::atomic<float> delay { 0.f };
    std::atomic<float> attack { 0.f };
    std::atomic<float> hold { 0.f };
    std::atomic<float> decay { 1.f };
    std::atomic<float> sustain { 1.f };
    std::atomic<float> release { 0.089f };

    std::array<DAHDSR, PortFrames::maxChannels> envelopes;
};