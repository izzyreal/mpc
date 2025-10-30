#include "EventRegistry.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include <algorithm>
#include <stdexcept>

using namespace mpc::eventregistry;
using namespace mpc::lcdgui;
using namespace mpc::sampler;
using namespace mpc::sequencer;

EventRegistry::EventRegistry()
{
    physicalPadPressEvents.reserve(CAPACITY);
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
    : physicalPadPressEvents(other.physicalPadPressEvents),
      programPadEvents(other.programPadEvents), noteEvents(other.noteEvents)
{
}

EventRegistry &EventRegistry::operator=(const EventRegistry &other) noexcept
{
    if (this != &other)
    {
        physicalPadPressEvents = other.physicalPadPressEvents;
        programPadEvents = other.programPadEvents;
        noteEvents = other.noteEvents;
    }
    return *this;
}

void EventRegistry::enqueue(EventMessage &&msg)
{
    queue.enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadPress(
    Source source, std::shared_ptr<ScreenComponent> screen,
    std::shared_ptr<Bus> bus, PhysicalPadIndex padIndex, Velocity velocity,
    Track *track, int bank, std::optional<int> note,
    std::function<void(void *)> action)
{
    printf("registering physical pad press\n");
    assert(screen && bus);
    auto e = std::make_shared<PhysicalPadPressEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->track = track;
    e->bus = bus;
    e->velocity = velocity;
    e->bank = bank;
    e->note = note;
    e->program = screen->getProgram();
    enqueue({EventMessage::Type::PhysicalPadPress, e, {}, {}, {}, {}, action});
}

void EventRegistry::registerPhysicalPadAftertouch(PhysicalPadIndex padIndex,
                                                  Pressure pressure)
{
    printf("registering physical pad aftertouch for pad index %i\n",
           padIndex.get());

    const auto snapshotView = getSnapshot();

    for (const auto &e : snapshotView.snap->physicalPadEvents)
    {
        if (e->padIndex == padIndex && e->source != Source::NoteRepeat)
        {
            enqueue({EventMessage::Type::PhysicalPadAftertouch,
                     e,
                     {},
                     {},
                     {},
                     pressure});
            return;
        }
    }

    // throw std::invalid_argument("No matching pad for aftertouch");
}

void EventRegistry::registerPhysicalPadRelease(
    PhysicalPadIndex padIndex, Source source,
    std::function<void(void *)> action)
{
    printf("registering physical pad release\n");
    auto e = std::make_shared<PhysicalPadReleaseEvent>(PhysicalPadReleaseEvent{padIndex, source});
    EventMessage msg;
    msg.type = EventMessage::Type::PhysicalPadRelease;
    msg.action = action;
    msg.physicalPadRelease = e;
    enqueue(std::move(msg));
}

ProgramPadEventPtr EventRegistry::registerProgramPadPress(
    Source source, std::shared_ptr<ScreenComponent> screen,
    std::shared_ptr<Bus> bus, std::shared_ptr<Program> program,
    ProgramPadIndex padIndex, Velocity velocity, Track *track,
    std::optional<MidiChannel> midiChannel)
{
    assert(screen && bus && program);
    printf("registering program pad press\n");

    auto e = std::make_shared<ProgramPadEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->track = track;
    e->bus = bus;
    e->program = program;
    e->velocity = velocity;
    e->midiChannel = midiChannel;
    enqueue({EventMessage::Type::ProgramPadPress, {}, e, {}});
    return e;
}

void EventRegistry::registerProgramPadAftertouch(
    Source source, std::shared_ptr<Bus> bus, std::shared_ptr<Program> program,
    ProgramPadIndex padIndex, Pressure pressure, Track *track)
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
                     {},
                     pressure});
            return;
        }
    }
}

void EventRegistry::registerProgramPadRelease(
    Source source, std::shared_ptr<Bus> bus, std::shared_ptr<Program> program,
    ProgramPadIndex padIndex, Track *track,
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
    Source source, std::shared_ptr<ScreenComponent> screen,
    std::shared_ptr<Bus> bus, NoteNumber noteNumber, Velocity velocity,
    Track *track, std::optional<MidiChannel> midiChannel,
    std::shared_ptr<Program> program)
{
    printf("registering note on\n");
    assert(screen && bus);
    auto e = std::make_shared<NoteEvent>();
    e->noteNumber = noteNumber;
    e->source = source;
    e->screen = screen;
    e->track = track;
    e->bus = bus;
    e->velocity = velocity;
    e->midiChannel = midiChannel;
    e->program = program;
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
            enqueue(
                {EventMessage::Type::NoteAftertouch, {}, {}, e, {}, pressure});
            return;
        }
    }
}

void EventRegistry::registerNoteOff(Source source, std::shared_ptr<Bus> bus,
                                    NoteNumber noteNumber, Track *track,
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
    physicalPadPressEvents.clear();
    programPadEvents.clear();
    noteEvents.clear();
    enqueue({EventMessage::Type::Clear, {}, {}, {}});
}

void EventRegistry::publishSnapshotToBuffer(Snapshot *dst) noexcept
{
    dst->physicalPadEvents.clear();
    dst->programPadEvents.clear();
    dst->noteEvents.clear();

    for (const auto &e : physicalPadPressEvents)
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
    Snapshot *cur =
        std::atomic_load_explicit(&snapshotPtr, std::memory_order_acquire);
    Snapshot *inactive = (cur == &snapA) ? &snapB : &snapA;

    publishSnapshotToBuffer(inactive);

    std::atomic_store_explicit(&snapshotPtr, inactive,
                               std::memory_order_release);
}

SnapshotView EventRegistry::getSnapshot() noexcept
{
    Snapshot *s =
        std::atomic_load_explicit(&snapshotPtr, std::memory_order_acquire);
    return SnapshotView(s);
}

void EventRegistry::drainQueue() noexcept
{
    EventMessage msg;
    while (queue.try_dequeue(msg))
    {
        applyMessage(msg);
    }
}

void EventRegistry::applyMessage(const EventMessage &msg) noexcept
{
    switch (msg.type)
    {
        case EventMessage::Type::PhysicalPadPress:
            printf("Applying PhysicalPadPress\n");
            assert(msg.physicalPadPress);
            msg.action(nullptr);
            physicalPadPressEvents.push_back(msg.physicalPadPress);
            break;

        case EventMessage::Type::PhysicalPadAftertouch:
            printf("Applying PhysicalPadAftertouch\n");
            assert(msg.physicalPadPress);
            assert(msg.pressure);
            msg.action(nullptr);
            msg.physicalPadPress->pressure = *msg.pressure;
            break;

        case EventMessage::Type::PhysicalPadRelease:
        {
            printf("Appying PhysicalPadRelease\n");
            assert(msg.physicalPadRelease);

            auto padPress = std::find_if(
                physicalPadPressEvents.begin(), physicalPadPressEvents.end(),
                [&](const auto &e)
                {
                    return e->padIndex == msg.physicalPadRelease->padIndex;
                });

            assert(padPress != physicalPadPressEvents.end());

            msg.action((void *)(*padPress).get());
            physicalPadPressEvents.erase(padPress);
            break;
        }
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
            physicalPadPressEvents.clear();
            programPadEvents.clear();
            noteEvents.clear();
            break;
    }
}
