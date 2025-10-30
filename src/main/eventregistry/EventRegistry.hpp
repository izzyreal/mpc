#pragma once

#include "ConstrainedInt.h"

#include <concurrentqueue.h>

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

    using PhysicalPadIndex = ConstrainedInt<uint8_t, 0, 15>;
    using Velocity = ConstrainedInt<uint8_t, 0, 127>;
    using ProgramPadIndex = ConstrainedInt<uint8_t, 0, 63>;
    using MidiChannel = ConstrainedInt<uint8_t, 0, 15>;
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
        mpc::sequencer::Track *track;
        std::shared_ptr<mpc::sequencer::Bus> bus;
        Velocity velocity;
        std::optional<Pressure> pressure;
        int bank{};
        std::optional<int> note;
        std::shared_ptr<mpc::sampler::Program> program;
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

    struct EventMessage
    {
        enum class Type
        {
            PhysicalPadPress,
            PhysicalPadAftertouch,
            PhysicalPadRelease,
            ProgramPadPress,
            ProgramPadAftertouch,
            ProgramPadRelease,
            NoteOn,
            NoteAftertouch,
            NoteOff,
            Clear
        } type;

        PhysicalPadEventPtr physicalPad;
        ProgramPadEventPtr programPad;
        NoteEventPtr noteEvent;

        std::optional<Pressure> pressure;
    };

    struct Snapshot
    {
        PhysicalPadEventPtrs physicalPadEvents;
        ProgramPadEventPtrs programPadEvents;
        NoteEventPtrs noteEvents;
    };

    class EventRegistry
    {
    public:
        EventRegistry();
        EventRegistry(const EventRegistry &other) noexcept;
        EventRegistry &operator=(const EventRegistry &other) noexcept;

        PhysicalPadEventPtr registerPhysicalPadPress(
            Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            std::shared_ptr<mpc::sequencer::Bus>, PhysicalPadIndex padIndex,
            Velocity, mpc::sequencer::Track *, int bank,
            std::optional<int> note);

        void registerPhysicalPadAftertouch(PhysicalPadIndex, Pressure);
        PhysicalPadEventPtr registerPhysicalPadRelease(PhysicalPadIndex);

        ProgramPadEventPtr registerProgramPadPress(
            Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            std::shared_ptr<mpc::sequencer::Bus>,
            std::shared_ptr<mpc::sampler::Program>, ProgramPadIndex padIndex,
            Velocity, sequencer::Track *, std::optional<MidiChannel>);

        void registerProgramPadAftertouch(
            Source, std::shared_ptr<mpc::sequencer::Bus>,
            std::shared_ptr<mpc::sampler::Program>, ProgramPadIndex padIndex,
            Pressure, sequencer::Track *);

        void registerProgramPadRelease(Source,
                                       std::shared_ptr<mpc::sequencer::Bus>,
                                       std::shared_ptr<mpc::sampler::Program>,
                                       ProgramPadIndex padIndex,
                                       sequencer::Track *,
                                       std::optional<MidiChannel>);

        NoteEventPtr
        registerNoteOn(Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
                       std::shared_ptr<mpc::sequencer::Bus>, NoteNumber,
                       Velocity, sequencer::Track *, std::optional<MidiChannel>,
                       std::shared_ptr<sampler::Program>);

        void registerNoteAftertouch(Source, NoteNumber, Pressure);
        void registerNoteOff(Source, std::shared_ptr<mpc::sequencer::Bus>,
                             NoteNumber, sequencer::Track *,
                             std::optional<MidiChannel>);

        void clear();

        class SnapshotView;
        SnapshotView getSnapshot() noexcept;

        // Called from the audio thread only
        void publishSnapshot() noexcept;

    private:
        const size_t CAPACITY = 8192;

        PhysicalPadEventPtrs physicalPadEvents;
        ProgramPadEventPtrs programPadEvents;
        NoteEventPtrs noteEvents;

        moodycamel::ConcurrentQueue<EventMessage> queue{512};

        Snapshot snapA;
        Snapshot snapB;
        alignas(void *) std::atomic<Snapshot *> snapshotPtr{&snapA};

        void enqueue(EventMessage &&);
        void publishSnapshotToBuffer(struct Snapshot *dst) noexcept;

        void drainQueue() noexcept;
        void applyMessage(const EventMessage &msg) noexcept;
    };

    class EventRegistry::SnapshotView
    {
    public:
        explicit SnapshotView(const Snapshot *s) noexcept : snap(s) {}

        bool isProgramPadPressedBySource(ProgramPadIndex idx, Source src) const;
        VelocityOrPressure
        getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex idx) const;
        bool isProgramPadPressed(ProgramPadIndex idx) const;
        int getTotalPressedProgramPadCount() const;

        NoteEventPtr retrievePlayNoteEvent(NoteNumber note) const;
        std::shared_ptr<sequencer::NoteOnEvent>
        retrieveRecordNoteEvent(NoteNumber note) const;
        PhysicalPadEventPtr
        retrievePhysicalPadEvent(PhysicalPadIndex idx) const;

        NoteEventPtr retrieveNoteEvent(NoteNumber note, Source src) const;

        bool valid() const noexcept
        {
            return snap != nullptr;
        }

        void printStats()
        {
            printf("===== EventRegistry Stats =======\n");
            printf("physicalPadEvent count: %zu\n", snap->physicalPadEvents.size());
            printf("programPadEvent  count: %zu\n", snap->programPadEvents.size());
            printf("noteEvent        count: %zu\n", snap->noteEvents.size());
            printf("=================================\n");
        }

    private:
        const Snapshot *snap{};
        friend class EventRegistry;
    };
} // namespace mpc::eventregistry
