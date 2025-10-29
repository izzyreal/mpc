#include "EventRegistry.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include <stdexcept>

using namespace mpc::eventregistry;

namespace
{
    template <typename... Events>
    void assertSameSource(Source expected, Events... events)
    {
        ((void)(
            [&]() {
                if constexpr (!std::is_same_v<Events, std::nullptr_t>)
                {
                    if (events) {
                        assert(events->source == expected);
                    }
                }
            }()),
         ...);
    }
} // namespace

EventRegistry::EventRegistry()
{
    physicalPadEvents.reserve(CAPACITY);
    programPadEvents.reserve(CAPACITY);
    noteEvents.reserve(CAPACITY);

    snapA.physicalPadEvents.reserve(CAPACITY);
    snapA.programPadEvents.reserve(CAPACITY);
    snapA.noteEvents.reserve(CAPACITY);

    snapB.physicalPadEvents.reserve(CAPACITY);
    snapB.programPadEvents.reserve(CAPACITY);
    snapB.noteEvents.reserve(CAPACITY);

    std::atomic_store_explicit(&snapshotPtr, &snapA, std::memory_order_release);
}

EventRegistry::EventRegistry(const EventRegistry &other) noexcept
    : physicalPadEvents(other.physicalPadEvents),
      programPadEvents(other.programPadEvents), noteEvents(other.noteEvents)
{
}

EventRegistry &EventRegistry::operator=(const EventRegistry &other) noexcept
{
    if (this != &other)
    {
        physicalPadEvents = other.physicalPadEvents;
        programPadEvents = other.programPadEvents;
        noteEvents = other.noteEvents;
    }
    return *this;
}

void EventRegistry::enqueue(EventMessage &&msg)
{
    // if (ctx == )
    // guiToAudio.enqueue(std::move(msg));
    // else
    // audioToGui.enqueue(std::move(msg));

    queue.enqueue(msg);
}

PhysicalPadEventPtr EventRegistry::registerPhysicalPadPress(
    Source source, std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus, PhysicalPadIndex padIndex,
    Velocity velocity, std::shared_ptr<sequencer::Track> track, int bank,
    std::optional<int> note)
{
    printf("registering physical pad press\n");
    assert(screen && bus);
    auto e = std::make_shared<PhysicalPadEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->track = track;
    e->bus = bus;
    e->velocity = velocity;
    e->bank = bank;
    e->note = note;
    e->program = screen->getProgram();
    enqueue({EventMessage::Type::PhysicalPadPress, e, {}, {}});
    return e;
}

void EventRegistry::registerPhysicalPadAftertouch(PhysicalPadIndex padIndex,
                                                  Pressure pressure)
{
    printf("registering physical pad aftertouch\n");

    const auto snapshotView = getSnapshot();

    for (const auto &e : snapshotView.snap->physicalPadEvents)
    {
        if (e->padIndex == padIndex && e->source != Source::NoteRepeat)
        {
            enqueue({EventMessage::Type::PhysicalPadAftertouch,
                     e,
                     {},
                     {},
                     pressure});
        }
    }

    throw std::invalid_argument("No matching pad for aftertouch");
}

PhysicalPadEventPtr
EventRegistry::registerPhysicalPadRelease(PhysicalPadIndex padIndex)
{
    printf("registering physical pad release\n");
    const auto snapshotView = getSnapshot();

    for (const auto &e : snapshotView.snap->physicalPadEvents)
    {
        if (e->padIndex == padIndex)
        {
            enqueue({EventMessage::Type::PhysicalPadRelease, e, {}, {}});
            return e;
        }
    }

    return {};
}

ProgramPadEventPtr EventRegistry::registerProgramPadPress(
    Source source, std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program, ProgramPadIndex padIndex,
    Velocity velocity, std::optional<TrackIndex> trackIndex,
    std::optional<MidiChannel> midiChannel)
{
    assert(screen && bus && program);
    assertSameSource(source);
    printf("registering program pad press\n");

    auto e = std::make_shared<ProgramPadEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->trackIndex = trackIndex;
    e->bus = bus;
    e->program = program;
    e->velocity = velocity;
    e->midiChannel = midiChannel;
    enqueue({EventMessage::Type::ProgramPadPress, {}, e, {}});
    return e;
}

void EventRegistry::registerProgramPadAftertouch(
    Source source, std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program, ProgramPadIndex padIndex,
    Pressure pressure, std::optional<TrackIndex> trackIndex)
{
    assert(bus && program);

    printf("registering program pad aftertouch\n");

    auto snapshot = getSnapshot();

    for (auto &e : snapshot.snap->programPadEvents)
    {
        if (e->padIndex == padIndex && e->source == source)
        {
            enqueue({EventMessage::Type::ProgramPadAftertouch,
                     {},
                     e,
                     {},
                     pressure});
            return;
        }
    }

    throw std::invalid_argument(
        "registerProgramPadAftertouch called without matching press");
}

void EventRegistry::registerProgramPadRelease(
    Source source, std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program, ProgramPadIndex padIndex,
    std::optional<TrackIndex> trackIndex,
    std::optional<MidiChannel> midiChannel)
{
    assert(bus && program);

    printf("registering program pad release\n");

    auto snapshot = getSnapshot();

    for (auto it = snapshot.snap->programPadEvents.rbegin();
         it != snapshot.snap->programPadEvents.rend(); ++it)
    {
        const auto &e = *it;
        if (e->padIndex == padIndex && e->source == source &&
            ((!midiChannel && !e->midiChannel) ||
             (midiChannel && e->midiChannel && *midiChannel == e->midiChannel)))
        {
            enqueue({EventMessage::Type::ProgramPadRelease, {}, e, {}});
            return;
        }
    }

    throw std::invalid_argument(
        "registerProgramPadRelease called without matching press");
}

NoteEventPtr EventRegistry::registerNoteOn(
    Source source, std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus, NoteNumber noteNumber,
    Velocity velocity, std::optional<TrackIndex> trackIndex,
    std::optional<MidiChannel> midiChannel)
{
    printf("registering note on\n");
    assert(screen && bus);
    auto e = std::make_shared<NoteEvent>();
    e->noteNumber = noteNumber;
    e->source = source;
    e->screen = screen;
    e->trackIndex = trackIndex;
    e->bus = bus;
    e->velocity = velocity;
    e->midiChannel = midiChannel;
    enqueue({EventMessage::Type::NoteOn, {}, {}, e});
    return e;
}

void EventRegistry::registerNoteAftertouch(Source source, NoteNumber noteNumber,
                                           Pressure pressure)
{
    printf("registering note aftertouch\n");
    auto snapshot = getSnapshot();

    for (auto &e : snapshot.snap->noteEvents)
    {
        if (e->noteNumber == noteNumber && e->source == source &&
            e->source != Source::NoteRepeat)
        {
            enqueue({EventMessage::Type::NoteAftertouch, {}, {}, e, pressure});
            return;
        }
    }

    throw std::invalid_argument(
        "registerNoteAftertouch called without matching note on");
}

void EventRegistry::registerNoteOff(Source source,
                                    std::shared_ptr<mpc::sequencer::Bus> bus,
                                    NoteNumber noteNumber,
                                    std::optional<TrackIndex> trackIndex,
                                    std::optional<MidiChannel> midiChannel)
{
    assert(bus);

    printf("registering note off\n");

    auto snapshot = getSnapshot();

    for (auto it = snapshot.snap->noteEvents.rbegin();
         it != snapshot.snap->noteEvents.rend(); ++it)
    {
        auto &e = *it;
        if (e->noteNumber == noteNumber && e->source == source &&
            ((!midiChannel && !e->midiChannel) ||
             (midiChannel && e->midiChannel && *midiChannel == e->midiChannel)))
        {
            enqueue({EventMessage::Type::NoteOff, {}, {}, e});
            return;
        }
    }

    throw std::invalid_argument(
        "registerNoteOff called without matching note on");
}

void EventRegistry::clear()
{
    physicalPadEvents.clear();
    programPadEvents.clear();
    noteEvents.clear();
    enqueue({EventMessage::Type::Clear, {}, {}, {}});
}

void EventRegistry::publishSnapshotToBuffer(Snapshot *dst) noexcept
{
    dst->physicalPadEvents.clear();
    dst->programPadEvents.clear();
    dst->noteEvents.clear();

    for (const auto &e : physicalPadEvents)
    {
        dst->physicalPadEvents.push_back(e);
    }

    for (const auto &e : programPadEvents)
    {
        dst->programPadEvents.push_back(e);
    }

    for (const auto &e : noteEvents)
    {
        dst->noteEvents.push_back(e);
    }
}

void EventRegistry::publishSnapshot() noexcept
{
    drainQueue();

    Snapshot *cur =
        std::atomic_load_explicit(&snapshotPtr, std::memory_order_acquire);
    Snapshot *inactive = (cur == &snapA) ? &snapB : &snapA;

    publishSnapshotToBuffer(inactive);

    std::atomic_store_explicit(&snapshotPtr, inactive,
                               std::memory_order_release);
}

EventRegistry::SnapshotView EventRegistry::getSnapshot() noexcept
{
    Snapshot *s =
        std::atomic_load_explicit(&snapshotPtr, std::memory_order_acquire);
    return SnapshotView(s);
}

bool EventRegistry::SnapshotView::isProgramPadPressedBySource(
    ProgramPadIndex idx, Source src) const
{
    for (auto &e : snap->programPadEvents)
    {
        if (e->padIndex == idx && e->source == src)
        {
            return true;
        }
    }
    return false;
}

VelocityOrPressure
EventRegistry::SnapshotView::getPressedProgramPadAfterTouchOrVelocity(
    ProgramPadIndex idx) const
{
    std::optional<VelocityOrPressure> result;

    for (const auto &e : snap->programPadEvents)
    {
        if (e->source == Source::NoteRepeat)
        {
            continue;
        }

        if (e->padIndex == idx)
        {
            if (e->pressure)
            {
                result = *e->pressure;
            }
            else if (!result)
            {
                result = e->velocity;
            }
        }
    }

    if (result)
    {
        return *result;
    }

    throw std::invalid_argument(
        "This method should only be called for program pads that are "
        "pressed, and the presses shouldn't be generated by the note repeat "
        "feature.\n");
}

bool EventRegistry::SnapshotView::isProgramPadPressed(ProgramPadIndex idx) const
{
    for (auto &e : snap->programPadEvents)
    {
        if (e->padIndex == idx)
        {
            return true;
        }
    }
    return false;
}

bool EventRegistry::SnapshotView::isAnyProgramPadPressed() const
{
    return !snap->programPadEvents.empty();
}

NoteEventPtr
EventRegistry::SnapshotView::retrievePlayNoteEvent(NoteNumber note) const
{
    for (auto &e : snap->noteEvents)
    {
        if (e->noteNumber == note)
        {
            return e;
        }
    }
    return nullptr;
}

std::shared_ptr<mpc::sequencer::NoteOnEvent>
EventRegistry::SnapshotView::retrieveRecordNoteEvent(NoteNumber note) const
{
    for (auto &e : snap->noteEvents)
    {
        if (e->noteNumber == note && e->recordNoteEvent)
        {
            return *e->recordNoteEvent;
        }
    }
    return nullptr;
}

PhysicalPadEventPtr EventRegistry::SnapshotView::retrievePhysicalPadEvent(
    PhysicalPadIndex idx) const
{
    for (auto &e : snap->physicalPadEvents)
    {
        if (e->padIndex == idx)
        {
            return e;
        }
    }
    return nullptr;
}

void EventRegistry::drainQueue() noexcept
{
    EventMessage msg;
    while (queue.try_dequeue(msg))
    {
        printf("Applying msg\n");
        applyMessage(msg);
    }
}

void EventRegistry::applyMessage(const EventMessage &msg) noexcept
{
    switch (msg.type)
    {
        case EventMessage::Type::PhysicalPadPress:
            assert(msg.physicalPad);
            physicalPadEvents.push_back(msg.physicalPad);
            printf("Applying PhysicalPadPress\n");
            break;

        case EventMessage::Type::PhysicalPadAftertouch:
            assert(msg.physicalPad);
            assert(msg.pressure);
            msg.physicalPad->pressure = *msg.pressure;
            printf("Applying PhysicalPadAftertouch\n");
            break;

        case EventMessage::Type::PhysicalPadRelease:
            assert(msg.physicalPad);
            printf("Appying PhysicalPadRelease\n");
            physicalPadEvents.erase(std::remove(physicalPadEvents.begin(),
                                                physicalPadEvents.end(),
                                                msg.physicalPad),
                                    physicalPadEvents.end());
            break;

        case EventMessage::Type::ProgramPadPress:
            assert(msg.programPad);
            printf("Appying ProgramPadPress\n");
            programPadEvents.push_back(msg.programPad);
            break;

        case EventMessage::Type::ProgramPadAftertouch:
            assert(msg.programPad);
            assert(msg.pressure);
            printf("Appying ProgramPadAftertouch\n");
            msg.programPad->pressure = *msg.pressure;
            break;

        case EventMessage::Type::ProgramPadRelease:
            assert(msg.programPad);
            printf("Appying ProgramPadRelease\n");
            programPadEvents.erase(std::remove(programPadEvents.begin(),
                                               programPadEvents.end(),
                                               msg.programPad),
                                   programPadEvents.end());
            break;

        case EventMessage::Type::NoteOn:
            assert(msg.noteEvent);
            printf("Appying NoteOn\n");
            noteEvents.push_back(msg.noteEvent);
            break;

        case EventMessage::Type::NoteAftertouch:
            assert(msg.noteEvent);
            assert(msg.pressure);
            printf("Appying NoteAftertouch\n");
            msg.noteEvent->pressure = *msg.pressure;
            break;

        case EventMessage::Type::NoteOff:
            assert(msg.noteEvent);
            printf("Appying NoteOff\n");
            noteEvents.erase(std::remove(noteEvents.begin(), noteEvents.end(),
                                         msg.noteEvent),
                             noteEvents.end());
            break;

        case EventMessage::Type::Clear:
            printf("Appying Clear\n");
            physicalPadEvents.clear();
            programPadEvents.clear();
            noteEvents.clear();
            break;
    }
}
