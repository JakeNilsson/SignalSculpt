#include "../helpers/SignalGraph.h"
#include "../modules/waveDSP.h"
#include "../modules/envDSP.h"
#include "../modules/filterDSP.h"
#include "../modules/staticIO.h"

SignalGraph::SignalGraph() {
    addStaticNode(audioIn, std::to_underlying(StaticIDs::AudIn), true, false);
    addStaticNode(audioOut, std::to_underlying(StaticIDs::AudOut), false, false);
    addStaticNode(noteIn, std::to_underlying(StaticIDs::NoteIn), true, true);
    addStaticNode(gateIn, std::to_underlying(StaticIDs::GateIn), true, true);
}

void SignalGraph::addStaticNode(StaticIO*& sink, int nodeID, bool isInput, bool isMidi) {;
    auto staticSink = std::make_unique<StaticIO>(nodeID, isInput, isMidi);
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
        } case ModuleType::Envelope : {
            nodes.add(
                new Node {
                    .id = counter,
                    .module = std::make_unique<EnvDSP>(sampleRate, counter)
                }
            );
            break;
        } case ModuleType::Filter : {
            nodes.add(
                new Node {
                    .id = counter,
                    .module = std::make_unique<FilterDSP>(sampleRate, counter)
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

void SignalGraph::connect(const int src_id, const int src_port, const int dst_id, const int dst_port) { // can take mod amount if needed
    const PortFrames *output = nullptr;

    for (const auto node : nodes) {
        if (node->id == src_id) {
            output = node->module->getOutput(src_port);
        }
    }

    if (output == nullptr) { jassertfalse; return; }

    for (const auto node : nodes) {
        if (node->id == dst_id) {
            if (!node->module->setInput(dst_port, output)) {
                jassertfalse;
            }

            node->module->recalcOutputChannels();
        }
    }
}

void SignalGraph::disconnect(const int src_id, const int src_port, const int dst_id, const int dst_port) {
    const PortFrames *output = nullptr;

    for (const auto node : nodes) {
        if (node->id == src_id) {
            output = node->module->getOutput(src_port);
        }
    }

    if (output == nullptr) { jassertfalse; return; }

    for (const auto node : nodes) {
        if (node->id == dst_id) {
            if (!node->module->removeInput(dst_port, output)) {
                jassertfalse;
            }

            node->module->recalcOutputChannels();
            return;
        }
    }
}

void SignalGraph::setModAmount(int src_id, int src_port, int dst_id, int dst_port, double modAmount) {
    const PortFrames *output = nullptr;

    for (const auto node : nodes) {
        if (node->id == src_id) {
            output = node->module->getOutput(src_port);
        }
    }

    if (output == nullptr) { jassertfalse; return; }

    for (const auto node : nodes) {
        if (node->id == dst_id) {
            if (!node->module->setModAmount(dst_port, output, modAmount)) {
                jassertfalse;
            }
        }
    }
}

void SignalGraph::setParameter(const int moduleId, const int paramId, const float value) {
    for (const auto node : nodes) {
        if (node->id == moduleId) {
            node->module->setParameter(paramId, value);
            return;
        }
    }
    jassertfalse;  // no module with the given ID
}

void SignalGraph::process(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    if (numChannels == 0) return;

    const float *leftR = buffer.getReadPointer(0);
    const float *rightR = numChannels > 1 ? buffer.getReadPointer(1) : nullptr;

    float* leftW = buffer.getWritePointer(0);
    float* rightW = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    auto it = midiMessages.begin();

    for (int i = 0; i < numSamples; ++i) {

        PortFrames pf;

        while (it != midiMessages.end() && (*it).samplePosition <= i) {
            if (const auto msg = (*it).getMessage(); msg.isNoteOn()) {
                for (int j = 0; j < PortFrames::maxChannels; ++j) {
                    if (noteToVoices[j] == msg.getNoteNumber() && static_cast<int>(gateIn->getOutputFrame(0, j)->left) == 0) {
                        gateIn->directInMono(j, 1);
                        skipMidiAssign = true;
                        break;
                    }
                }
                if (!skipMidiAssign) {
                    noteIn->directInMono (midiCh, static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (msg.getNoteNumber())) / 20000);
                    noteToVoices[midiCh] = msg.getNoteNumber();
                    gateIn->directInMono (midiCh, 1);
                    midiCh = (midiCh + 1) % PortFrames::maxChannels;
                }
                skipMidiAssign = false;

            } else if (const auto msg = (*it).getMessage(); msg.isNoteOff()) {
                for (int j = 0; j < PortFrames::maxChannels; ++j) {
                    if (noteToVoices[j] == msg.getNoteNumber() && static_cast<int>(gateIn->getOutputFrame(0, j)->left) != 0) {
                        gateIn->directInMono(j, 0);
                        break;
                    }
                }
            }

            ++it;
        }

        const float left  = leftR[i];
        const float right = rightR != nullptr ? rightR[i] : left;

        audioIn->directInStereo (static_cast<int>(StaticIDs::AudIn), { .left = left, .right = right });

        for (const auto node : nodes){
            node->module->process();
        }

        const Frame* f = audioOut->getOutputFrame(StaticIO::staticOut, 0);
        leftW[i] = f->left;
        if (rightW != nullptr) rightW[i] = f->right;
    }
}