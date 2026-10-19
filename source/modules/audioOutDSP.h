#pragma once
#include "moduleDSP.h"
#include "../helpers/moduleIDs.h"

class AudioOutDSP : public ModuleDSP {
public:
    enum Input  { audioIn, numInputPorts };
    enum Output { audioOut, numOutputPorts };
    AudioOutDSP() : ModuleDSP (numInputPorts, numOutputPorts) { setModuleID (AUD); }
    void tick() noexcept override { out (audioOut, inStereo (audioIn)); }
};