#pragma once

#include <cmath>

inline bool nearly_equal(const double x, const double y, const double epsilon = 0.001) {
    return std::abs(x - y) <= epsilon;
}

class DAHDSR {
public:
    enum Segments {
        Delay,
        Attack,
        Hold,
        Decay,
        Sustain,
    };

    float getNextValue() {
        if (gate) { // skips segment if their time is set to 0
            if (currSegment == Delay && nearly_equal(delayStepCount, 0.f)) {
                currSegment = Attack;
                progress = 1.f;
            } if (currSegment == Attack && nearly_equal(attackStepCount, 0.f)) {
                currSegment = Hold;
                signal = 1.f;
                progress = 2.f;
            } if (currSegment == Hold && nearly_equal(holdStepCount, 0.f)) {
                currSegment = Decay;
                signal = 1.f;
                progress = 3.f;
            } if (currSegment == Decay && nearly_equal(decayStepCount, 0.f)) {
                currSegment = Sustain;
                signal = sustain;
                progress = 4.f;
            }

            switch (currSegment) {
                case Delay : {
                    progress += (1 / delayStepCount);

                    if (signal > 0.f) {
                        signal -= releaseStepJump;
                    } else {
                        signal = 0.f;
                    }

                    if (progress >= 1.f) {
                        progress = 1.f;
                        currSegment = Attack;
                    }

                    break;
                } case Attack : {
                    progress += (1 / attackStepCount);

                    if (!attackHit) {

                        if (attackStepCount == 0.f) {
                            jassertfalse;
                        }

                        attackStepJump = (1 - signal) / attackStepCount;
                        attackHit = true;
                    }

                    signal += attackStepJump;

                    if (progress >= 2.f) {
                        progress = 2.f;
                        currSegment = Hold;
                    }

                    break;
                } case Hold : {
                    progress += (1 / holdStepCount);
                    signal = 1.f;

                    if (progress >= 3.f) {
                        progress = 3.f;
                        currSegment = Decay;
                    }

                    break;
                } case Decay : {
                    progress += (1 / decayStepCount);
                    signal -= decayStepJump;

                    if (progress >= 4.f) {
                        progress = 4.f;
                        currSegment = Sustain;
                    }

                    break;
                } case Sustain : {
                    signal = sustain;
                    break;
                }
            }
        } else {
            progress = 0.f;
            currSegment = Delay;

            if (signal > 0.f) {
                signal -= releaseStepJump;
            } else {
                signal = 0.f;
            }
        }

        return signal;
    }

    [[nodiscard]] bool DAHDSRChanged(const float del, const float atk, const float hld, const float dec, const float sus, const float rel) const {
        return !(nearly_equal(delay, del) &&
                 nearly_equal(attack, atk) &&
                 nearly_equal(hold, hld) &&
                 nearly_equal(decay, dec) &&
                 nearly_equal(sustain, sus) &&
                 nearly_equal(release, rel));
    }

    bool gateChanged() {
        if (oldGate != gate) {
            oldGate = gate;
            return true;
        }

        return false;
    }

    void gateOn() {
        gate = true;

        if (gateChanged()) {
            currSegment = Delay;
            progress = 0.f;
            attackHit = false;
        }
    }

    void gateOff() {
        gate = false;

        if (gateChanged()) {
            if (releaseStepCount != 0.f) {
                releaseStepJump = signal / releaseStepCount;
            } else {
                releaseStepJump = signal;
            }
        }
    }

    void setSampleRate(const double newSampleRate) {
        sampleRate = newSampleRate;

        setSteps();
    }

    void setDAHDSR(const float del, const float atk, const float hld, const float dec, const float sus, const float rel) {
        delay = del;
        attack = atk;
        hold = hld;
        decay = dec;
        sustain = sus;
        release = rel;

        setSteps();
    }

private:
    void setSteps() {
        delayStepCount = (delay * static_cast<float>(sampleRate));
        attackStepCount = (attack * static_cast<float>(sampleRate));
        holdStepCount = (hold * static_cast<float>(sampleRate));
        decayStepCount = (decay * static_cast<float>(sampleRate));
        releaseStepCount = (release * static_cast<float>(sampleRate));

        if (attackStepCount != 0.f) { // arbitrary setters, not expected to be used if attackStepSize == 0 anyways;
            attackStepJump = 1 / attackStepCount;
        } else {
            attackStepJump = 1;
        }

        if (decayStepCount != 0.f) {
            decayStepJump = (1 - sustain) / decayStepCount;
        } else {
            decayStepJump = 1;
        }

    }

    double sampleRate = 44100.0;

    float progress = 0.f;
    float signal = 0.f;
    Segments currSegment = Delay;

    float delay = 0.f;
    float attack = 0.f;
    float hold = 0.f;
    float decay = 1.f;
    float sustain = 1.f;
    float release = 0.089f;

    float delayStepCount = 0.f;
    float attackStepCount = 0.f;
    float holdStepCount = 0.f;
    float decayStepCount = 0.f;
    float releaseStepCount = 0.f;

    float attackStepJump = 1.f;
    float decayStepJump = 0.f;
    float releaseStepJump = 1.f; // I'm thinking this can get calculated on gateOff();

    bool gate = false;
    bool oldGate = false;

    bool attackHit = false;
};