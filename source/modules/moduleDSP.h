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

    [[nodiscard]] int getNumInputs() const noexcept {return static_cast<int>(inputs.size());}
    [[nodiscard]] int getNumOutputs() const noexcept {return static_cast<int>(outputs.size());}

    [[nodiscard]] const Frame* getOutput (const int port) const noexcept {
        if (port < 0 || port >= getNumOutputs())
            return nullptr;
        return &outputs[static_cast<std::size_t> (port)];
    }

    [[nodiscard]] bool setInput (const int port, const Frame* source) noexcept {
        if (port < 0 || port >= getNumInputs())
            return false;                                   // no such port

        inputs[static_cast<std::size_t> (port)] = source;   // nullptr is a valid "disconnect"
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
        inputs(static_cast<std::size_t>(std::max(0, numInputs)), nullptr),
        outputs(static_cast<std::size_t>(std::max(0, numOutputs)))
    {}

    [[nodiscard]] Frame inStereo (const int port) const noexcept {
        assert (port >= 0 && port < getNumInputs());
        const Frame* src = inputs[static_cast<std::size_t>(port)];
        return src != nullptr ? *src : Frame{};
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
    virtual void onPrepare() {} // to be appended by derived classes

private:
    std::vector<const Frame*> inputs;
    std::vector<Frame> outputs;

    double sampleRate = 44100.0;
    int moduleID = 0;
};