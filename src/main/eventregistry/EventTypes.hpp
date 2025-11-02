#pragma once

#include "eventregistry/Source.hpp"

#include "IntTypes.hpp"

#include <vector>
#include <memory>
#include <optional>
#include <chrono>

namespace mpc::sequencer
{
    class Track;
    class Bus;
    class NoteOnEvent;
} // namespace mpc::sequencer

namespace mpc::sampler
{
    class Sampler;
    class Program;
} // namespace mpc::sampler

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
        int bank{};
        std::shared_ptr<mpc::sampler::Program> program;
        std::optional<int> note;
        std::optional<Pressure> pressure;
    };

    struct PhysicalPadAftertouchEvent
    {
        PhysicalPadIndex padIndex;
        Pressure pressure;
    };

    struct PhysicalPadReleaseEvent
    {
        PhysicalPadIndex padIndex;
    };

    struct ProgramPadPressEvent
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
        std::chrono::steady_clock::time_point pressTime;
    };

    struct ProgramPadAftertouchEvent
    {
        ProgramPadIndex padIndex;
        std::shared_ptr<mpc::sampler::Program> program;
        Pressure pressure;
    };

    struct ProgramPadReleaseEvent
    {
        ProgramPadIndex padIndex;
        std::shared_ptr<mpc::sampler::Program> program;
    };

    struct NoteOnEvent
    {
        NoteNumber noteNumber;
        Source source{};
        std::optional<MidiChannel> midiChannel;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        mpc::sequencer::Track *track;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        Velocity velocity;
        std::optional<std::shared_ptr<mpc::sequencer::NoteOnEvent>>
            recordNoteEvent;
        std::shared_ptr<mpc::sampler::Program> program;
        std::optional<Pressure> pressure;
    };

    struct NoteAftertouchEvent
    {
        NoteNumber noteNumber;
        Pressure pressure;
        std::optional<MidiChannel> midiChannel;
    };

    struct NoteOffEvent
    {
        NoteNumber noteNumber;
        std::optional<MidiChannel> midiChannel;
    };

    using PhysicalPadPressEventPtr = std::shared_ptr<PhysicalPadPressEvent>;
    using PhysicalPadAftertouchEventPtr =
        std::shared_ptr<PhysicalPadAftertouchEvent>;
    using PhysicalPadReleaseEventPtr = std::shared_ptr<PhysicalPadReleaseEvent>;
    using ProgramPadPressEventPtr = std::shared_ptr<ProgramPadPressEvent>;
    using ProgramPadAftertouchEventPtr =
        std::shared_ptr<ProgramPadAftertouchEvent>;
    using ProgramPadReleaseEventPtr = std::shared_ptr<ProgramPadReleaseEvent>;
    using NoteOnEventPtr = std::shared_ptr<NoteOnEvent>;
    using NoteAftertouchEventPtr = std::shared_ptr<NoteAftertouchEvent>;
    using NoteOffEventPtr = std::shared_ptr<NoteOffEvent>;

    using PhysicalPadPressEventPtrs = std::vector<PhysicalPadPressEventPtr>;
    using ProgramPadPressEventPtrs = std::vector<ProgramPadPressEventPtr>;
    using NoteOnEventPtrs = std::vector<NoteOnEventPtr>;
} // namespace mpc::eventregistry
