#pragma once

#include <vector>
#include <atomic>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <juce_audio_basics/juce_audio_basics.h>

struct Frame {float left = 0.0f; float right = 0.0f; };

struct PortFrames {
    static constexpr int maxChannels = 8; // maybe make this a user-settable variable later?
    std::array<Frame, maxChannels> channels {};
    int numChannels = 1;
};

struct Cable {
    static constexpr double rampSeconds = 0.02;

    const PortFrames* source = nullptr;
    std::atomic<double> mod { 1.f };
    juce::LinearSmoothedValue<float> smoothedMod;
    float current = 1.f;

    Cable (const PortFrames* s, double m, double sampleRate) noexcept : source (s), mod (m) {
        smoothedMod.reset(sampleRate, rampSeconds);
        smoothedMod.setCurrentAndTargetValue(static_cast<float>(m));
        current = static_cast<float>(m);
    }

    // std::atomic can't be copied, so copy its value by hand.
    // This is what lets push_back / erase work.
    Cable (const Cable& o) noexcept
        : source (o.source), mod (o.mod.load (std::memory_order_relaxed)),
          smoothedMod(o.smoothedMod), current(o.current){}

    Cable& operator= (const Cable& o) noexcept {
        source = o.source;
        mod.store (o.mod.load (std::memory_order_relaxed), std::memory_order_relaxed);
        smoothedMod = o.smoothedMod;
        current = o.current;
        return *this;
    }
};

class ModuleDSP {
public:
    virtual ~ModuleDSP() = default;

    ModuleDSP (const ModuleDSP&) = delete;

    ModuleDSP& operator= (const ModuleDSP&) = delete;

    void prepare (const double newSampleRate) {
        sampleRate = newSampleRate;
        for (auto& port : inputs)
            for (auto& cable : port)
                cable.smoothedMod.reset (sampleRate, Cable::rampSeconds);
        onPrepare();
    }
    virtual void setParameter (int /*paramId*/, float /*value*/) noexcept {}

    [[nodiscard]] int getNumInputs() const noexcept {return static_cast<int>(inputs.size());}
    [[nodiscard]] int getNumCables(const int id) const noexcept {return static_cast<int>(inputs[static_cast<size_t>(id)].size());}
    [[nodiscard]] int getNumOutputs() const noexcept {return static_cast<int>(outputs.size());}
    [[nodiscard]] static int getMaxChannels() noexcept {return PortFrames::maxChannels;}

    [[nodiscard]] int inChannels(const int port) const {
        assert(port >= 0 && port < getNumInputs());

        int channelCount = 1;

        for (const auto& cable : inputs[static_cast<size_t>(port)]) {
            channelCount = std::max(channelCount, cable.source->numChannels);
        }

        return channelCount;
    }

    [[nodiscard]] const PortFrames* getOutput (const int port) const noexcept {
        if (port < 0 || port >= getNumOutputs())
            return nullptr;
        return &outputs[static_cast<std::size_t> (port)];
    }

    [[nodiscard]] const Frame* getOutputFrame (const int port, const int channel) const noexcept {
        if (port < 0 || port >= getNumOutputs() || channel < 0 || channel >= outputs[static_cast<size_t>(port)].numChannels)
            return nullptr;
        return &outputs[static_cast<std::size_t> (port)].channels[static_cast<size_t>(channel)];
    }

    [[nodiscard]] bool setInput (const int port, const PortFrames* source, const double modAmount = 1) noexcept {
        if (port < 0 || port >= getNumInputs())
            return false;                                   // no such port

        inputs[static_cast<std::size_t> (port)].emplace_back(source, modAmount, sampleRate);
        return true;
    }

    [[nodiscard]] bool removeInput (const int port, const PortFrames* source) noexcept {
        if (port < 0 || port >= getNumInputs())
            return false;                                   // no such port

        auto& cables = inputs[static_cast<size_t>(port)];
        const auto it = std::ranges::find(cables, source, &Cable::source);

        if (it == cables.end()) return false;
        cables.erase (it);
        return true;
    }

    [[nodiscard]] bool setModAmount (const int port, const PortFrames* source, const double modAmount) noexcept {
        if (port < 0 || port >= getNumInputs())
            return false;

        auto& cables = inputs[static_cast<size_t> (port)];
        const auto it = std::ranges::find (cables, source, &Cable::source);

        if (it == cables.end()) return false;
        it->mod.store (modAmount, std::memory_order_relaxed);
        return true;
    }

    [[nodiscard]] int getOutputChannels() const noexcept {
        return outputChannels;
    }

    void recalcOutputChannels() noexcept {
        int n = 1;
        for (int p = 0; p < getNumInputs(); ++p)
            n = std::max (n, inChannels (p));
        outputChannels = n;
    }

    void setOutputChannels(const int newOutputChannels) noexcept {
        outputChannels = newOutputChannels;
    }

    void process() noexcept {
        for (auto& port : inputs)
            for (auto& cable : port) {
                cable.smoothedMod.setTargetValue (static_cast<float> (cable.mod.load (std::memory_order_relaxed)));
                cable.current = cable.smoothedMod.getNextValue();
            }
        tick();
    }

    virtual void tick() noexcept = 0;

    [[nodiscard]] int getModuleID() const {
        return moduleID;
    }

    void setModuleID(const int newModuleID) noexcept {
        moduleID = newModuleID;
    }

protected:
    ModuleDSP(const int numInputs, const int numOutputs) :
        inputs  (static_cast<std::size_t> (std::max (0, numInputs))),
        outputs (static_cast<std::size_t> (std::max (0, numOutputs)))
    {}

    void setOutChannels(const int port, const int numChannels) noexcept {
        outputs[static_cast<size_t>(port)].numChannels = std::clamp (numChannels, 1, PortFrames::maxChannels);
    }

    virtual void onPrepare() {} // to be appended by derived classes

    [[nodiscard]] Frame inStereo (const int port, int channel) const noexcept {
        assert (port >= 0 && port < getNumInputs() &&
                channel >= 0 && channel < getMaxChannels());
        Frame sum = {};

        for (const auto& cable : inputs[static_cast<size_t>(port)]) {
            const auto* src = cable.source;
            const int ch = channel < src->numChannels ? channel : channel % src->numChannels;
            const float m = cable.current;

            sum.left  += src->channels[static_cast<size_t> (ch)].left  * static_cast<float>(m);
            sum.right += src->channels[static_cast<size_t> (ch)].right * static_cast<float>(m);
        }
        return sum;
    }

    [[nodiscard]] float inMono (const int port, const int channel) const noexcept {
        const auto [left, right] = inStereo (port, channel);
        return 0.5f * (left + right);
    }

    void out (const int port, const int channel, const float mono) noexcept {
        assert (port >= 0 && port < getNumOutputs() &&
                channel >= 0 && channel < outputs[static_cast<size_t>(port)].numChannels);

        outputs[static_cast<size_t>(port)].channels[static_cast<size_t>(channel)] = {.left = mono, .right = mono};
    }

    void out (const int port, const int channel, const Frame frame) noexcept {
        assert (port >= 0 && port < getNumOutputs() && channel < outputs[static_cast<size_t>(port)].numChannels);
        outputs[static_cast<size_t>(port)].channels[static_cast<size_t>(channel)] = frame;
    }

    void out (const int port, const PortFrames& portFrames) noexcept {
        assert (port >= 0 && port < getNumOutputs());
        outputs[static_cast<std::size_t>(port)] = portFrames;
    }

    [[nodiscard]] double getSampleRate() const noexcept { return sampleRate; }

private:
    std::vector<std::vector<Cable>>  inputs;
    std::vector<PortFrames> outputs;

    double sampleRate = 44100.0;
    int moduleID = -1;

    int outputChannels = 1;
};