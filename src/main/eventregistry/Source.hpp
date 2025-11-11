#pragma once

#include <string>

namespace mpc::eventregistry
{
    enum class Source
    {
        VirtualMpcHardware,
        Sequence,
        MidiInput,
        StepEditor,
        NoteRepeat
    };

    static std::string sourceToString(Source s)
    {
        if (s == Source::VirtualMpcHardware)
        {
            return "VirtualMpcHardware";
        }
        if (s == Source::Sequence)
        {
            return "Sequence";
        }
        if (s == Source::MidiInput)
        {
            return "MidiInput";
        }
        if (s == Source::StepEditor)
        {
            return "StepEditor";
        }
        if (s == Source::NoteRepeat)
        {
            return "NoteRepeat";
        }
        return "Unknown";
    }
} // namespace mpc::eventregistry
