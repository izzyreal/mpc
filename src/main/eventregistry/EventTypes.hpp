#pragma once

#include "eventregistry/Source.hpp"

#include "IntTypes.hpp"

#include <memory>
#include <optional>

namespace mpc::sequencer
{
    class Track;
    class Bus;
    class NoteOnEvent;
}

namespace mpc::sampler
{
    class Sampler;
    class Program;
}

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::eventregistry
{
    struct PhysicalPadPressEvent
    {
        PhysicalPadIndex padIndex;
        Source source{};
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        mpc::sequencer::Track *track;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        Velocity velocity;
        std::optional<Pressure> pressure;
        int bank{};
        std::optional<int> note;
        std::shared_ptr<mpc::sampler::Program> program;
    };

    struct PhysicalPadReleaseEvent
    {
        PhysicalPadIndex padIndex;
        Source source{};
    };

    struct ProgramPadEvent
    {
        ProgramPadIndex padIndex;
        Source source{};
        std::optional<MidiChannel> midiChannel;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        mpc::sequencer::Track *track;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        std::shared_ptr<mpc::sampler::Program> program;
        Velocity velocity;
        std::optional<Pressure> pressure;
    };

    struct NoteEvent
    {
        NoteNumber noteNumber;
        Source source{};
        std::optional<MidiChannel> midiChannel;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        mpc::sequencer::Track *track;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        std::optional<Velocity> velocity;
        std::optional<Pressure> pressure;
        std::optional<std::shared_ptr<mpc::sequencer::NoteOnEvent>>
            recordNoteEvent;
        std::shared_ptr<mpc::sampler::Program> program;
    };

    using PhysicalPadPressEventPtr = std::shared_ptr<PhysicalPadPressEvent>;
    using PhysicalPadReleaseEventPtr = std::shared_ptr<PhysicalPadReleaseEvent>;
    using ProgramPadEventPtr = std::shared_ptr<ProgramPadEvent>;
    using NoteEventPtr = std::shared_ptr<NoteEvent>;
} // namespace mpc::eventregistry
