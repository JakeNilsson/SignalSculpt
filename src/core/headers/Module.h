#pragma once
#include <string>

class Module {
    public:
    Module(const std::string& name) : m_name(name), m_id(next_id++) {}
    virtual ~Module() = default;

    virtual void draw()    = 0;  // UI Rendering
    virtual void process() = 0;  // DSP through JUCE

    const std::string& getName() const { return m_name; }
    std::string windowTitle() const { return m_name + "##" + std::to_string(m_id); }
    bool isOpen() { return m_open; }

    protected:
    std::string m_name;
    bool m_open = true;
    int m_id;
    static int next_id;
};