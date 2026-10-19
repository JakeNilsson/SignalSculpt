#include "../helpers/SignalGraph.h"
#include "../modules/waveDSP.h"

SignalGraph::SignalGraph() {
    auto sink = std::make_unique<AudioOutDSP>();
    audioOut = sink.get();                                        // grab the raw pointer first
    nodes.add (new Node { .id = AUD, .module = std::move (sink) });
}

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

void SignalGraph::connect(const int src_id, const int src_port, const int dst_id, const int dst_port) {
    const Frame *output = nullptr;

    for (const auto node : nodes) {
        if (node->id == src_id) {
            output = node->module->getOutput(src_port);
        }
    }

    for (const auto node : nodes) {
        if (node->id == dst_id) {
            if (!node->module->setInput(dst_port, output)) {
                jassertfalse;
            }
        }
    }
}

void SignalGraph::process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) const {
    juce::ignoreUnused(midiMessages);
    buffer.clear();

    const auto numChannels = buffer.getNumChannels();
    if (numChannels == 0) return;

    float* left = buffer.getWritePointer(0);
    float* right = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < buffer.getNumSamples(); i++) {
        for (auto node : nodes) {
            node->module->tick();
        }

        const Frame* f = audioOut->getOutput(AudioOutDSP::audioOut);
        left[i] = f->left;
        if (right != nullptr) right[i] = f->right;
    }
}
