#pragma once

#include "moduleDSP.h"

class FilterDSP : public ModuleDSP {
public:
    enum Input  { filterIn, filterTypeIn, filterSpecIn, numInputPorts };
    enum Output { filterOut, numOutputPorts };

    FilterDSP(const double sampleRate, const int ID) : ModuleDSP(numInputPorts, numOutputPorts) {
        prepare(sampleRate);
        setModuleID(ID);
    }

    void setParameter(int, float) noexcept override {

    }

    void tick() noexcept override {

    }

    void onPrepare() noexcept override {

    }

private:

};