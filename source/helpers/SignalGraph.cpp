#include "../helpers/SignalGraph.h"
#include "../modules/waveDSP.h"
#include "../modules/staticIO.h"

SignalGraph::SignalGraph() {
    addStaticNode(audioIn, std::to_underlying(StaticIDs::AudIn), true);
    addStaticNode(audioOut, std::to_underlying(StaticIDs::AudOut), false);
    addStaticNode(midiNoteIn, std::to_underlying(StaticIDs::MidiIn), true);
}

void SignalGraph::addStaticNode(ModuleDSP*& sink, int nodeID, bool isInput) {;
    auto staticSink = std::make_unique<StaticIO>(nodeID, isInput);
    sink = staticSink.get();                                        // grab the raw pointer first
    nodes.add (new Node { .id = nodeID, .module = std::move (staticSink) });
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

void SignalGraph::setParameter(int moduleId, int paramId, float value) {
    for (const auto node : nodes) {
        if (node->id == moduleId) {
            node->module->setParameter(paramId, value);
            return;
        }
    }
    jassertfalse;  // no module with the given ID
}

void SignalGraph::process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) const {

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (numChannels == 0) return;

    const float *leftR = buffer.getReadPointer(0);
    const float *rightR = numChannels > 1 ? buffer.getReadPointer(1) : nullptr;

    float* leftW = buffer.getWritePointer(0);
    float* rightW = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    auto it = midiMessages.begin();

    for (int i = 0; i < numSamples; ++i) {
        while (it != midiMessages.end() && (*it).samplePosition <= i) {
            if (const auto msg = (*it).getMessage(); msg.isNoteOn()) {
                midiNoteIn->directInMono (static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber())) / 20000);
            }
            ++it;
        }

        const float left  = leftR[i];
        const float right = rightR != nullptr ? rightR[i] : left;

        audioIn->directInStereo ({ .left = left, .right = right });

        for (auto node : nodes) {
            node->module->tick();
        }

        const Frame* f = audioOut->getOutput(StaticIO::staticOut);
        leftW[i] = f->left;
        if (rightW != nullptr) rightW[i] = f->right;
    }
}