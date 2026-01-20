#pragma once

namespace mpc::performance
{
    enum class PerformanceEventSource
    {
        VirtualMpcHardware,
        Sequence,
        MidiInput,
        NoteRepeat
    };

    /*
    static std::string sourceToString(const Source s)
    {
        if (s == PerformanceEventSource::VirtualMpcHardware)
        {
            return "VirtualMpcHardware";
        }
        if (s == PerformanceEventSource::Sequence)
        {
            return "Sequence";
        }
        if (s == PerformanceEventSource::MidiInput)
        {
            return "MidiInput";
        }
        if (s == PerformanceEventSource::NoteRepeat)
        {
            return "NoteRepeat";
        }
        return "Unknown";
    }
    */
} // namespace mpc::performance
