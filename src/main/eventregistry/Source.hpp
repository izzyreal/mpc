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
        else if (s == Source::Sequence)
        {
            return "Sequence";
        }
        else if (s == Source::MidiInput)
        {
            return "MidiInput";
        }
        else if (s == Source::StepEditor)
        {
            return "StepEditor";
        }
        else if (s == Source::NoteRepeat)
        {
            return "NoteRepeat";
        }
        return "Unknown";
    }
} // namespace mpc::eventregistry
