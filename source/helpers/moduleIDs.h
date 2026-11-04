#pragma once

enum class ModuleType {
    Wave,
    Envelope,
};

enum class StaticIDs : int {
    AudIn,
    AudOut,
    NoteIn,
    NoteOut,
    GateIn,
    GateOut,
    VelIn,
    VelOut,
    StaticCount,
};