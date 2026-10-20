#pragma once

#include <memory>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "moduleIDs.h"
#include "../modules/moduleDSP.h"
#include "../modules/audioOutDSP.h"

struct Node { int id; std::unique_ptr<ModuleDSP> module; };
struct dspConnection { int srcId; int srcPort; int dstId; int dstPort; };

class SignalGraph {
public:
    SignalGraph();

    void prepare(const double newSampleRate) {
        sampleRate = newSampleRate;
        for (const auto node : nodes) {
            node->module->prepare(newSampleRate);
        }
    }

    [[nodiscard]] double getSampleRate() const {
        return sampleRate;
    }

    void setParameter (int moduleId, int paramId, float value);

    void process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) const;

    int addModule(ModuleType moduleType);

    void connect(int src_id, int src_port, int dst_id, int dst_port);

private:
    void incrementCounter() {
        counter++;
    }

    double sampleRate = 44100.0;
    int counter = 0;

    juce::OwnedArray<Node> nodes;
    juce::OwnedArray<dspConnection> connections;

    ModuleDSP* audioOut = nullptr;
};
