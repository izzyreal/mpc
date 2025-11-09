#pragma once

#include "IntTypes.hpp"

#include "eventregistry/Source.hpp"
#include "eventregistry/EventTypes.hpp"
#include "eventregistry/EventMessage.hpp"

#include "eventregistry/SnapshotView.hpp"

#include <memory>
#include <optional>
#include <functional>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel

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
    class EventRegistry
    {
        using EventMessageQueue = moodycamel::ConcurrentQueue<
            EventMessage, moodycamel::ConcurrentQueueDefaultTraits>;

    public:
        EventRegistry();

        void registerPhysicalPadPress(
            Source, const std::shared_ptr<lcdgui::ScreenComponent> &,
            const std::shared_ptr<sequencer::Bus> &, PhysicalPadIndex padIndex,
            Velocity, sequencer::Track *, int bank, std::optional<int> note,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadAftertouch(
            PhysicalPadIndex, Pressure, Source source,
            const std::function<void(void *)> &action) const;

        void registerPhysicalPadRelease(
            PhysicalPadIndex, Source source,
            const std::function<void(void *)> &action) const;

        ProgramPadPressEventPtr registerProgramPadPress(
            Source, const std::shared_ptr<lcdgui::ScreenComponent> &,
            const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<sampler::Program> &, ProgramPadIndex padIndex,
            Velocity, sequencer::Track *, std::optional<MidiChannel>) const;

        void registerProgramPadAftertouch(
            Source, const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<sampler::Program> &, ProgramPadIndex padIndex,
            Pressure, sequencer::Track *) const;

        void registerProgramPadRelease(
            Source, const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<sampler::Program> &, ProgramPadIndex padIndex,
            sequencer::Track *, std::optional<MidiChannel>,
            const std::function<void(void *)> &action) const;

        NoteOnEventPtr
        registerNoteOn(Source, const std::shared_ptr<lcdgui::ScreenComponent> &,
                       const std::shared_ptr<sequencer::Bus> &, NoteNumber,
                       Velocity, sequencer::Track *, std::optional<MidiChannel>,
                       const std::shared_ptr<sampler::Program> &,
                       const std::function<void(void *)> &action) const;

        void registerNoteAftertouch(Source, NoteNumber, Pressure,
                                    std::optional<MidiChannel>) const;
        void registerNoteOff(Source, 
                             NoteNumber,
                             std::optional<MidiChannel>,
                             const std::function<void(void *)> &action) const;

        void clear();

        SnapshotView getSnapshot() const noexcept;

        // Called from the audio thread only
        void drainQueue() noexcept;
        void publishSnapshot() noexcept;

    private:
        const size_t CAPACITY = 8192;

        PhysicalPadPressEventPtrs physicalPadEvents;
        ProgramPadPressEventPtrs programPadEvents;
        NoteOnEventPtrs noteEvents;

        std::shared_ptr<EventMessageQueue> eventMessageQueue;

        Snapshot snapA, snapB;
        Snapshot* writeTarget;
        std::shared_ptr<Snapshot> currentSnapshot;

        void enqueue(EventMessage &&) const;
        void publishSnapshotToBuffer(struct Snapshot *dst) const noexcept;

        void applyMessage(const EventMessage &msg) noexcept;
    };
} // namespace mpc::eventregistry
