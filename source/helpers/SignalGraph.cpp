#include "../helpers/SignalGraph.h"

int SignalGraph::addModule(const ModuleType moduleType) {
    counter++;

    switch (moduleType) {
        case ModuleType::Wave : {
            nodes.add(
                new Node {
                    .id = counter,
                    .module = std::make_unique<WaveDSP>(sampleRate, counter)
                }
            );
            break;
        }

        default : {
            return -1;
        }
    }

    return counter;
}

void SignalGraph::process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    juce::ignoreUnused(midiMessages);

    float out = 0.f;
    const auto numChannels = buffer.getNumChannels();

    buffer.clear();

    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        out = 0.f;

        for (const auto node : nodes) {
            node->module->tick();

            out += node->module->getOutput(0)->left;
        }

        for (int j = 0; j < numChannels; j++) {
            buffer.setSample(j, i, out);
        }
    }
}
