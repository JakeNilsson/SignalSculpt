#pragma once
#include "moduleDSP.h"
#include "../helpers/moduleIDs.h"

class StaticIO : public ModuleDSP {
public:
    enum Input  { staticIn, numInputPorts };
    enum Output { staticOut, numOutputPorts };

    explicit StaticIO(const int modID, const bool isInput) : ModuleDSP (numInputPorts, numOutputPorts) {
        setModuleID (modID);

        if (isInput) {
            if (!setInput (staticIn, getOutput(staticOut))) {
                jassertfalse;
            }
        }
    }

    void tick() noexcept override { out (staticOut, inStereo (staticIn));}
};