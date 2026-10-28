#pragma once
#include "moduleDSP.h"
#include "../helpers/moduleIDs.h"

class StaticIO : public ModuleDSP {
public:
    enum Input  { staticIn, numInputPorts };
    enum Output { staticOut, numOutputPorts };
    explicit StaticIO(const int moduleID) : ModuleDSP (numInputPorts, numOutputPorts) { setModuleID (moduleID); }
    void tick() noexcept override { out (staticOut, inStereo (staticIn)); }
};