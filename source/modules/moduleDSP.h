#pragma once
#include <vector>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

struct Frame {float left = 0.0f; float right = 0.0f; };

struct PortFrames {
    static constexpr int maxChannels = 8; // maybe make this a user-settable variable later?
    std::array<Frame, maxChannels> channels {};
    int numChannels = 1;
};

class ModuleDSP {
public:
    virtual ~ModuleDSP() = default;

    ModuleDSP (const ModuleDSP&) = delete;

    ModuleDSP& operator= (const ModuleDSP&) = delete;

    void prepare(const double newSampleRate) {
        sampleRate = newSampleRate;
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

        for (const auto& input : inputs[static_cast<size_t>(port)]) {
            channelCount = std::max(channelCount, input->numChannels);
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

    [[nodiscard]] bool setInput (const int port, const PortFrames* source) noexcept {
        if (port < 0 || port >= getNumInputs())
            return false;                                   // no such port

        inputs[static_cast<std::size_t> (port)].push_back(source);   // nullptr is a valid "disconnect"
        return true;
    }

    [[nodiscard]] int getOutputChannels() const noexcept {
        return outputChannels;
    }

    void setOutputChannels(const int newOutputChannels) noexcept {
        outputChannels = newOutputChannels;
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
        inputs(static_cast<std::size_t>(std::max(0, numInputs)),
            std::vector<const PortFrames*>(0, nullptr)),
        outputs(static_cast<std::size_t>(std::max(0, numOutputs)))
    {}

    void setOutChannels(const int port, const int numChannels) noexcept {
        outputs[static_cast<size_t>(port)].numChannels = std::max(std::min(1, numChannels), PortFrames::maxChannels);
    }

    virtual void onPrepare() {} // to be appended by derived classes

    [[nodiscard]] Frame inStereo (const int port, const int channel) const noexcept {
        assert (port >= 0 && port < getNumInputs() &&
                channel >= 0 && channel < getMaxChannels());
        Frame sum = {.left = 0, .right = 0};
        for (const auto input : inputs[static_cast<size_t>(port)]) {
            if (input->numChannels <= channel) continue;

            sum.left += input->channels[static_cast<size_t>(channel)].left;
            sum.right += input->channels[static_cast<size_t>(channel)].right;
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
    std::vector<std::vector<const PortFrames*>> inputs;
    std::vector<PortFrames> outputs;

    double sampleRate = 44100.0;
    int moduleID = -1;

    int outputChannels = 1;
};