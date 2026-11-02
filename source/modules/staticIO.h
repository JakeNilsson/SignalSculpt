#pragma once
#include "moduleDSP.h"

class StaticIO : public ModuleDSP {
public:
    enum Input  { staticIn, numInputPorts };
    enum Output { staticOut, numOutputPorts };

    explicit StaticIO(const int modID, const bool isInput, const bool isMidi) : ModuleDSP (numInputPorts, numOutputPorts) {
        setModuleID (modID);
        isSource = isInput;
        isMultiChannelOut = isMidi;

        if (isMidi) setChannels (PortFrames::maxChannels);
    }

    void tick() noexcept override {
        if (isSource) return;
        if (isMultiChannelOut) {
            const int n = inChannels (staticIn);
            setOutChannels (staticOut, n);
            for (int i = 0; i < inChannels(staticIn); i++) {
                out (staticOut, i, inStereo (staticIn, i));
            }
        } else {
            Frame mix;
            for (int c = 0; c < inChannels (staticIn); ++c) {
                const auto [left, right] = inStereo (staticIn, c);
                mix.left += left;  mix.right += right;
            }
            out (staticOut, 0, mix);
        }
    }

    void setChannels (const int n) noexcept { setOutChannels (staticOut, n); }

    void directInPortFrames(const PortFrames& portFrames) noexcept {out (0, portFrames);}
    void directInStereo(const int channel, const Frame frame) noexcept { out(0, channel, frame); }
    void directInMono(const int channel, const float signal) noexcept { out(0, channel, signal); }

private:
    bool isSource;
    bool isMultiChannelOut;
};