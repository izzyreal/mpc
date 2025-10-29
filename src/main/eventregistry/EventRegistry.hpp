#pragma once

#include "ConstrainedInt.h"

#include <memory>
#include <vector>
#include <optional>
#include <cassert>
#include <atomic>

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
} // namespace mpc::sequencer

namespace mpc::eventregistry
{
    struct ProgramPadEvent;
    struct PhysicalPadEvent;
    struct NoteEvent;

    using PhysicalPadEventPtr = std::shared_ptr<PhysicalPadEvent>;
    using ProgramPadEventPtr = std::shared_ptr<ProgramPadEvent>;
    using NoteEventPtr = std::shared_ptr<NoteEvent>;

    using PhysicalPadEventPtrs = std::vector<PhysicalPadEventPtr>;
    using ProgramPadEventPtrs = std::vector<ProgramPadEventPtr>;
    using NoteEventPtrs = std::vector<NoteEventPtr>;

    struct Snapshot
    {
        PhysicalPadEventPtrs physicalPadEvents;
        ProgramPadEventPtrs programPadEvents;
        NoteEventPtrs noteEvents;
    };

    using PhysicalPadIndex = ConstrainedInt<uint8_t, 0, 15>;
    using Velocity = ConstrainedInt<uint8_t, 0, 127>;
    using ProgramPadIndex = ConstrainedInt<uint8_t, 0, 63>;
    using MidiChannel = ConstrainedInt<uint8_t, 0, 15>;
    using TrackIndex = ConstrainedInt<uint8_t, 0, 63>;
    using Pressure = ConstrainedInt<uint8_t, 0, 127>;
    using NoteNumber = ConstrainedInt<uint8_t, 0, 127>;
    using DrumNoteNumber = ConstrainedInt<uint8_t, 34, 98>;
    using VelocityOrPressure = ConstrainedInt<uint8_t, 0, 127>;

    enum class Source
    {
        VirtualMpcHardware,
        Sequence,
        MidiInput,
        StepEditor,
        NoteRepeat
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
        ProgramPadIndex padIndex;
        Source source{};
        std::optional<MidiChannel> midiChannel;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screen;
        std::optional<TrackIndex> trackIndex;
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

        EventRegistry(const EventRegistry& other) noexcept
            : physicalPadEvents(other.physicalPadEvents),
              programPadEvents(other.programPadEvents),
              noteEvents(other.noteEvents)
        {
        }

        EventRegistry& operator=(const EventRegistry& other) noexcept
        {
            if (this != &other)
            {
                physicalPadEvents = other.physicalPadEvents;
                programPadEvents  = other.programPadEvents;
                noteEvents        = other.noteEvents;
            }
            return *this;
        }

        PhysicalPadEventPtr registerPhysicalPadPress(
            Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            std::shared_ptr<mpc::sequencer::Bus>, PhysicalPadIndex padIndex,
            Velocity, std::shared_ptr<sequencer::Track>, int bank,
            std::optional<int> note);

        void registerPhysicalPadAftertouch(PhysicalPadIndex padIndex, Pressure);

        PhysicalPadEventPtr
        registerPhysicalPadRelease(PhysicalPadIndex padIndex);

        ProgramPadEventPtr registerProgramPadPress(
            Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            std::shared_ptr<mpc::sequencer::Bus>,
            std::shared_ptr<mpc::sampler::Program>, ProgramPadIndex padIndex,
            Velocity, std::optional<TrackIndex>, std::optional<MidiChannel>);

        void registerProgramPadAftertouch(
            Source, std::shared_ptr<mpc::sequencer::Bus>,
            std::shared_ptr<mpc::sampler::Program>, ProgramPadIndex padIndex,
            Pressure, std::optional<TrackIndex>);

        void registerProgramPadRelease(Source,
                                       std::shared_ptr<mpc::sequencer::Bus>,
                                       std::shared_ptr<mpc::sampler::Program>,
                                       ProgramPadIndex padIndex,
                                       std::optional<TrackIndex>,
                                       std::optional<MidiChannel>);

        NoteEventPtr registerNoteOn(
            Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            std::shared_ptr<mpc::sequencer::Bus>, NoteNumber, Velocity,
            std::optional<TrackIndex>, std::optional<MidiChannel>);

        void registerNoteAftertouch(Source source, NoteNumber, Pressure);

        void registerNoteOff(Source, std::shared_ptr<mpc::sequencer::Bus>,
                             NoteNumber, std::optional<TrackIndex>,
                             std::optional<MidiChannel>);

        bool isProgramPadPressedBySource(ProgramPadIndex, Source) const;
        VelocityOrPressure
            getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex) const;
        bool isProgramPadPressed(ProgramPadIndex padIndex) const;
        bool isAnyProgramPadPressed() const;

        void clear();

        NoteEventPtr retrievePlayNoteEvent(NoteNumber) const;
        std::shared_ptr<sequencer::NoteOnEvent>
            retrieveRecordNoteEvent(NoteNumber) const;
        PhysicalPadEventPtr retrievePhysicalPadEvent(PhysicalPadIndex) const;

        // snapshot management
        void publishSnapshot() noexcept;
        class SnapshotView;
        SnapshotView getSnapshot() const noexcept;

    private:
        void publishSnapshotToBuffer(struct Snapshot *dst) noexcept;

        const size_t CAPACITY = 8192;

        PhysicalPadEventPtrs physicalPadEvents;
        ProgramPadEventPtrs programPadEvents;
        NoteEventPtrs noteEvents;

        // two internal buffers that live for the lifetime of the registry
        Snapshot snapA;
        Snapshot snapB;

        // pointer to the active snapshot (either &snapA or &snapB)
        alignas(void*) std::atomic<Snapshot*> snapshotPtr{&snapA};
    };

        // Immutable view that mirrors EventRegistry's read-only API
    class EventRegistry::SnapshotView
    {
    public:
        explicit SnapshotView(const Snapshot* s) noexcept : snap(s) {}

        bool isProgramPadPressedBySource(ProgramPadIndex idx, Source src) const;
        VelocityOrPressure getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex idx) const;
        bool isProgramPadPressed(ProgramPadIndex idx) const;
        bool isAnyProgramPadPressed() const;

        NoteEventPtr retrievePlayNoteEvent(NoteNumber note) const;
        std::shared_ptr<sequencer::NoteOnEvent> retrieveRecordNoteEvent(NoteNumber note) const;
        PhysicalPadEventPtr retrievePhysicalPadEvent(PhysicalPadIndex idx) const;

        bool valid() const noexcept { return snap != nullptr; }

    private:
        const Snapshot* snap{};
    };
} // namespace mpc::eventregistry

