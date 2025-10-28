#pragma once

#include "ConstrainedInt.h"

#include <memory>
#include <vector>
#include <optional>
#include <cassert>

namespace mpc::sampler
{
    class Program;
}

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::sequencer
{
    class Track;
    class Bus;
    class NoteOnEvent;
}

namespace mpc::eventregistry
{
    struct ProgramPadEvent;
    struct PhysicalPadEvent;
    struct NoteEvent;

    using PhysicalPadEventPtr = std::shared_ptr<PhysicalPadEvent>;
    using ProgramPadEventPtr  = std::shared_ptr<ProgramPadEvent>;
    using NoteEventPtr        = std::shared_ptr<NoteEvent>;

    using PhysicalPadEventPtrs = std::vector<PhysicalPadEventPtr>;
    using ProgramPadEventPtrs  = std::vector<ProgramPadEventPtr>;
    using NoteEventPtrs        = std::vector<NoteEventPtr>;

    using PhysicalPadIndex = ConstrainedInt<uint8_t, 0, 15>;
    using Velocity = ConstrainedInt<uint8_t, 0, 127>;
    using ProgramPadIndex = ConstrainedInt<uint8_t, 0, 63>;
    using MidiChannel = ConstrainedInt<uint8_t, 0, 15>;
    using TrackIndex = ConstrainedInt<uint8_t, 0, 63>;
    using Pressure = ConstrainedInt<uint8_t, 0, 127>;
    using NoteNumber = ConstrainedInt<uint8_t, 0, 127>;
    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;

    enum class Source
    {
        VirtualMpcHardware,
        Sequence,
        MidiInput,
        AdhocGenerated
    };

    struct PhysicalPadEvent
    {
        PhysicalPadIndex padIndex;
        Source source{};
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        std::shared_ptr<sequencer::Track> track;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        Velocity velocity;
        std::optional<Pressure> pressure;
        int bank;
        std::optional<int> note;
        std::shared_ptr<sampler::Program> program;
    };

    struct ProgramPadEvent
    {
        enum class Type { Press, Aftertouch, Release };
        ProgramPadIndex padIndex;
        Type type{};
        Source source{};
        std::optional<MidiChannel> midiChannel;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        std::optional<TrackIndex> trackIndex;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        std::shared_ptr<mpc::sampler::Program> program;
        std::optional<Velocity> velocity;
        std::optional<Pressure> pressure;
    };

    struct NoteEvent
    {
        enum class Type { On, Aftertouch, Off };
        NoteNumber noteNumber;
        Type type{};
        Source source{};
        std::optional<MidiChannel> midiChannel;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        std::optional<TrackIndex> trackIndex;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        std::optional<Velocity> velocity;
        std::optional<Pressure> pressure;
        std::optional<std::shared_ptr<sequencer::NoteOnEvent>> recordNoteEvent;
    };

    class EventRegistry
    {
    public:
        EventRegistry();

        PhysicalPadEventPtr registerPhysicalPadPress(Source source,
                                                            std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
                                                            std::shared_ptr<mpc::sequencer::Bus> bus,
                                                            PhysicalPadIndex padIndex,
                                                            Velocity velocity,
                                                            std::shared_ptr<sequencer::Track> track,
                                                            int bank,
                                                            std::optional<int> note);

        void registerPhysicalPadAftertouch(Source source,
                                                                 std::shared_ptr<mpc::sequencer::Bus> bus,
                                                                 PhysicalPadIndex padIndex,
                                                                 Pressure pressure,
                                                                 std::optional<TrackIndex> trackIndex);

        PhysicalPadEventPtr registerPhysicalPadRelease(PhysicalPadIndex padIndex);

        ProgramPadEventPtr registerNonMidiProgramPadPress(Source source,
                                                          std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
                                                          std::shared_ptr<mpc::sequencer::Bus> bus,
                                                          std::shared_ptr<mpc::sampler::Program> program,
                                                          ProgramPadIndex padIndex,
                                                          Velocity velocity,
                                                          std::optional<TrackIndex> trackIndex);

        void registerNonMidiProgramPadAftertouch(Source source,
                                                               std::shared_ptr<mpc::sequencer::Bus> bus,
                                                               std::shared_ptr<mpc::sampler::Program> program,
                                                               ProgramPadIndex padIndex,
                                                               Pressure pressure,
                                                               std::optional<TrackIndex> trackIndex);

        void registerNonMidiProgramPadRelease(Source source,
                                                            std::shared_ptr<mpc::sequencer::Bus> bus,
                                                            std::shared_ptr<mpc::sampler::Program> program,
                                                            ProgramPadIndex padIndex,
                                                            std::optional<TrackIndex> trackIndex);

        NoteEventPtr registerNonMidiNoteOn(Source source,
                                           std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
                                           std::shared_ptr<mpc::sequencer::Bus> bus,
                                           NoteNumber noteNumber,
                                           Velocity velocity,
                                           std::optional<TrackIndex> trackIndex);

        void registerNonMidiNoteAftertouch(Source source,
                                                   std::shared_ptr<mpc::sequencer::Bus> bus,
                                                   NoteNumber noteNumber,
                                                   Pressure pressure,
                                                   std::optional<TrackIndex> trackIndex);

        void registerNonMidiNoteOff(Source source,
                                            std::shared_ptr<mpc::sequencer::Bus> bus,
                                            NoteNumber noteNumber,
                                            std::optional<TrackIndex> trackIndex);

        ProgramPadEventPtr registerMidiProgramPadPress(Source source,
                                                       std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
                                                       std::shared_ptr<mpc::sequencer::Bus> bus,
                                                       std::shared_ptr<mpc::sampler::Program> program,
                                                       ProgramPadIndex padIndex,
                                                       Velocity velocity,
                                                       std::optional<MidiChannel> midiChannel,
                                                       std::optional<TrackIndex> trackIndex);

        void registerMidiProgramPadAftertouch(Source source,
                                                            std::shared_ptr<mpc::sequencer::Bus> bus,
                                                            std::shared_ptr<mpc::sampler::Program> program,
                                                            ProgramPadIndex padIndex,
                                                            Pressure pressure,
                                                            std::optional<MidiChannel> midiChannel,
                                                            std::optional<TrackIndex> trackIndex);

        void registerMidiProgramPadRelease(Source source,
                                                         std::shared_ptr<mpc::sequencer::Bus> bus,
                                                         std::shared_ptr<mpc::sampler::Program> program,
                                                         ProgramPadIndex padIndex,
                                                         std::optional<MidiChannel> midiChannel,
                                                         std::optional<TrackIndex> trackIndex);

        NoteEventPtr registerMidiNoteOn(Source source,
                                        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
                                        std::shared_ptr<mpc::sequencer::Bus> bus,
                                        NoteNumber noteNumber,
                                        Velocity velocity,
                                        std::optional<MidiChannel> midiChannel,
                                        std::optional<TrackIndex> trackIndex);

        void registerMidiNoteAftertouch(Source source,
                                                std::shared_ptr<mpc::sequencer::Bus> bus,
                                                NoteNumber noteNumber,
                                                Pressure pressure,
                                                std::optional<MidiChannel> midiChannel,
                                                std::optional<TrackIndex> trackIndex);

        void registerMidiNoteOff(Source source,
                                         std::shared_ptr<mpc::sequencer::Bus> bus,
                                         NoteNumber noteNumber,
                                         std::optional<MidiChannel> midiChannel,
                                         std::optional<TrackIndex> trackIndex);

        bool isProgramPadPressedBySource(ProgramPadIndex padIndex, Source source) const;
        std::optional<int> getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex padIndex) const;
        bool isProgramPadPressed(ProgramPadIndex padIndex) const;
        bool isAnyProgramPadPressed() const;

        void clear();

        NoteEventPtr retrievePlayNoteEvent(NoteNumber noteNumber) const;
        std::shared_ptr<sequencer::NoteOnEvent> retrieveRecordNoteEvent(NoteNumber noteNumber) const;

    private:
        const size_t CAPACITY = 8192;

        PhysicalPadEventPtrs physicalPadEvents;
        ProgramPadEventPtrs programPadEvents;
        NoteEventPtrs noteEvents;
    };
}

