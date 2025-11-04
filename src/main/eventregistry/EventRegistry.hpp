#pragma once

#include "IntTypes.hpp"

#include "eventregistry/Source.hpp"
#include "eventregistry/EventTypes.hpp"
#include "eventregistry/EventMessage.hpp"

#include "eventregistry/SnapshotView.hpp"

#include <memory>
#include <optional>
#include <cassert>
#include <atomic>
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
    public:
        EventRegistry();
        EventRegistry(const EventRegistry &other) noexcept;
        EventRegistry &operator=(const EventRegistry &other) noexcept;

        void registerPhysicalPadPress(
            Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            std::shared_ptr<mpc::sequencer::Bus>, PhysicalPadIndex padIndex,
            Velocity, mpc::sequencer::Track *, int bank,
            std::optional<int> note, std::function<void(void *)> action);

        void registerPhysicalPadAftertouch(PhysicalPadIndex, Pressure,
                                           Source source,
                                           std::function<void(void *)> action);

        void registerPhysicalPadRelease(PhysicalPadIndex, Source source,
                                        std::function<void(void *)> action);

        ProgramPadPressEventPtr registerProgramPadPress(
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
                                       std::optional<MidiChannel>,
                                       std::function<void(void *)> action);

        NoteOnEventPtr
        registerNoteOn(Source, std::shared_ptr<mpc::lcdgui::ScreenComponent>,
                       std::shared_ptr<mpc::sequencer::Bus>, NoteNumber,
                       Velocity, sequencer::Track *, std::optional<MidiChannel>,
                       std::shared_ptr<sampler::Program>);

        void registerNoteAftertouch(Source, NoteNumber, Pressure,
                                    std::optional<MidiChannel>);
        void registerNoteOff(Source, std::shared_ptr<mpc::sequencer::Bus>,
                             NoteNumber, sequencer::Track *,
                             std::optional<MidiChannel>);

        void clear();

        SnapshotView getSnapshot() noexcept;

        // Called from the audio thread only
        void drainQueue() noexcept;
        void publishSnapshot() noexcept;

    private:
        const size_t CAPACITY = 8192;

        PhysicalPadPressEventPtrs physicalPadEvents;
        ProgramPadPressEventPtrs programPadEvents;
        NoteOnEventPtrs noteEvents;

        std::shared_ptr<moodycamel::ConcurrentQueue<
            EventMessage, moodycamel::ConcurrentQueueDefaultTraits>>
            queue;

        Snapshot snapA;
        Snapshot snapB;
        alignas(void *) std::atomic<Snapshot *> snapshotPtr{&snapA};

        void enqueue(EventMessage &&);
        void publishSnapshotToBuffer(struct Snapshot *dst) noexcept;

        void applyMessage(const EventMessage &msg) noexcept;
    };
} // namespace mpc::eventregistry
