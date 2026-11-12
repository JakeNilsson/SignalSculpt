#pragma once

enum class ModuleType : int {
    None,
    Wave,
    Envelope,
    Filter,
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