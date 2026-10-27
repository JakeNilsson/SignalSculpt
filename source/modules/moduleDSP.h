#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstddef>

struct Frame {float left = 0.0f; float right = 0.0f; };

class ModuleDSP {
public:
    virtual ~ModuleDSP() = default;

    ModuleDSP (const ModuleDSP&) = delete;

    ModuleDSP& operator= (const ModuleDSP&) = delete;

    void prepare(const double newSampleRate) {
        sampleRate = newSampleRate;
        onPrepare();
    }

    virtual void onPrepare() {} // to be appended by derived classes

    virtual void setParameter (int /*paramId*/, float /*value*/) noexcept {}

    [[nodiscard]] int getNumInputs() const noexcept {return static_cast<int>(inputs.size());}
    [[nodiscard]] int getNumInputsInputs(const size_t id) const noexcept {return static_cast<int>(inputs[id].size());}
    [[nodiscard]] int getNumOutputs() const noexcept {return static_cast<int>(outputs.size());}

    [[nodiscard]] const Frame* getOutput (const int port) const noexcept {
        if (port < 0 || port >= getNumOutputs())
            return nullptr;
        return &outputs[static_cast<std::size_t> (port)];
    }

    [[nodiscard]] bool setInput (const int port, const Frame* source) noexcept {
        if (port < 0 || port >= getNumInputs())
            return false;                                   // no such port

        inputs[static_cast<std::size_t> (port)].push_back(source);   // nullptr is a valid "disconnect"
        return true;
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
            std::vector<const Frame*>(0, nullptr)),
        outputs(static_cast<std::size_t>(std::max(0, numOutputs)))
    {}

    [[nodiscard]] Frame inStereo (const int port) const noexcept {
        assert (port >= 0 && port < getNumInputs());
        Frame sum = {.left = 0, .right = 0};
        for (const auto input : inputs[static_cast<size_t>(port)]) {
            sum.left += input->left;
            sum.right += input->right;
        }
        const auto src = inputs[static_cast<std::size_t>(port)];
        return src.empty() ? Frame{} : sum;
    }

    [[nodiscard]] float inMono (const int port) const noexcept { return inStereo (port).left; }

    void out (const int port, const float mono) noexcept {
        assert (port >= 0 && port < getNumOutputs());
        outputs[static_cast<std::size_t>(port)] = { .left = mono, .right = mono };
    }

    void out (const int port, const Frame stereo) noexcept {
        assert (port >= 0 && port < getNumOutputs());
        outputs[static_cast<std::size_t>(port)] = stereo;
    }

    [[nodiscard]] double getSampleRate() const noexcept { return sampleRate; }

private:
    std::vector<std::vector<const Frame*>> inputs;
    std::vector<Frame> outputs;

    double sampleRate = 44100.0;
    int moduleID = 0;
};