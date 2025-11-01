#include "EventRegistry.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include <algorithm>

using namespace mpc::eventregistry;
using namespace mpc::lcdgui;
using namespace mpc::sampler;
using namespace mpc::sequencer;

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
    queue.enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadPress(
    Source source, std::shared_ptr<ScreenComponent> screen,
    std::shared_ptr<Bus> bus, PhysicalPadIndex padIndex, Velocity velocity,
    Track *track, int bank, std::optional<int> note,
    std::function<void(void *)> action)
{
    //printf("registering physical pad press\n");
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

    EventMessage msg{EventMessage::Type::PhysicalPadPress};
    msg.physicalPadPress = e;
    msg.action = action;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadAftertouch(PhysicalPadIndex padIndex,
                                                  Pressure pressure,
                                                  Source source,
                                                  std::function<void(void*)> action)
{
    //printf("registering physical pad aftertouch for pad index %i\n", padIndex.get());

    auto e = std::make_shared<PhysicalPadAftertouchEvent>(
        PhysicalPadAftertouchEvent{padIndex, pressure});
    EventMessage msg{EventMessage::Type::PhysicalPadAftertouch};
    msg.physicalPadAftertouch = e;
    msg.action = action;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadRelease(
    PhysicalPadIndex padIndex, Source source,
    std::function<void(void *)> action)
{
    //printf("registering physical pad release\n");
    auto e = std::make_shared<PhysicalPadReleaseEvent>(
        PhysicalPadReleaseEvent{padIndex});
    EventMessage msg;
    msg.type = EventMessage::Type::PhysicalPadRelease;
    msg.action = action;
    msg.source = source;
    msg.physicalPadRelease = e;
    enqueue(std::move(msg));
}

ProgramPadPressEventPtr EventRegistry::registerProgramPadPress(
    Source source, std::shared_ptr<ScreenComponent> screen,
    std::shared_ptr<Bus> bus, std::shared_ptr<Program> program,
    ProgramPadIndex padIndex, Velocity velocity, Track *track,
    std::optional<MidiChannel> midiChannel)
{
    assert(screen && bus && program);
    //printf("registering program pad press with source %s\n", sourceToString(source).c_str());

    auto e = std::make_shared<ProgramPadPressEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->track = track;
    e->bus = bus;
    e->program = program;
    e->velocity = velocity;
    e->midiChannel = midiChannel;
    e->pressTime = std::chrono::steady_clock::now();

    EventMessage msg{EventMessage::Type::ProgramPadPress};
    msg.programPadPress = e;
    msg.source = source;
    enqueue(std::move(msg));
    return e;
}

void EventRegistry::registerProgramPadAftertouch(
    Source source, std::shared_ptr<Bus> bus, std::shared_ptr<Program> program,
    ProgramPadIndex padIndex, Pressure pressure, Track *track)
{
    assert(bus && program);

    //printf("registering program pad aftertouch\n");

    auto e = std::make_shared<ProgramPadAftertouchEvent>(
        ProgramPadAftertouchEvent{padIndex, program, pressure});
    EventMessage msg{EventMessage::Type::ProgramPadAftertouch};
    msg.programPadAftertouch = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerProgramPadRelease(
    Source source, std::shared_ptr<Bus> bus, std::shared_ptr<Program> program,
    ProgramPadIndex padIndex, Track *track,
    std::optional<MidiChannel> midiChannel, std::function<void(void *)> action)
{
    assert(bus && program);
    //printf("registering program pad release with source %s\n", sourceToString(source).c_str());

    auto e = std::make_shared<ProgramPadReleaseEvent>(
        ProgramPadReleaseEvent{padIndex, program});
    EventMessage msg;
    msg.type = EventMessage::Type::ProgramPadRelease;
    msg.source = source;
    msg.action = action;
    msg.programPadRelease = e;
    enqueue(std::move(msg));
}

NoteOnEventPtr EventRegistry::registerNoteOn(
    Source source, std::shared_ptr<ScreenComponent> screen,
    std::shared_ptr<Bus> bus, NoteNumber noteNumber, Velocity velocity,
    Track *track, std::optional<MidiChannel> midiChannel,
    std::shared_ptr<Program> program)
{
    //printf("registering note on\n");
    assert(screen && bus);
    auto e = std::make_shared<NoteOnEvent>(
        NoteOnEvent{noteNumber, source, midiChannel, screen, track, bus,
                    velocity, nullptr, program, std::nullopt});
    EventMessage msg{EventMessage::Type::NoteOn};
    msg.noteOnEvent = e;
    msg.source = source;
    enqueue(std::move(msg));
    return e;
}

void EventRegistry::registerNoteAftertouch(Source source, NoteNumber noteNumber,
                                           Pressure pressure,
                                           std::optional<MidiChannel> midiChannel)
{
    //printf("registering note aftertouch\n");
    auto e = std::make_shared<NoteAftertouchEvent>(
        NoteAftertouchEvent{noteNumber, pressure, midiChannel});
    EventMessage msg{EventMessage::Type::NoteAftertouch};
    msg.source = source;
    msg.noteAftertouchEvent = e;
    enqueue(std::move(msg));
}

void EventRegistry::registerNoteOff(Source source, std::shared_ptr<Bus> bus,
                                    NoteNumber noteNumber, Track *track,
                                    std::optional<MidiChannel> midiChannel)
{
    assert(bus);

    //printf("registering note off\n");

    auto e =
        std::make_shared<NoteOffEvent>(NoteOffEvent{noteNumber, midiChannel});
    EventMessage msg{EventMessage::Type::NoteOff};
    msg.noteOffEvent = e;
    msg.source = source;
    enqueue(std::move(msg));
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
            //printf("Applying PhysicalPadPress\n");
            assert(msg.physicalPadPress);
            physicalPadEvents.push_back(msg.physicalPadPress);
            publishSnapshot();
            msg.action(msg.physicalPadPress.get());
            break;

        case EventMessage::Type::PhysicalPadAftertouch:
            //printf("Applying PhysicalPadAftertouch\n");
            for (auto &p : physicalPadEvents)
            {
                if (p->padIndex == msg.physicalPadAftertouch->padIndex &&
                        p->source == msg.source)
                {
                    p->pressure = msg.physicalPadAftertouch->pressure;
                    publishSnapshot();
                    msg.action(p.get());
                }
            }

            break;

        case EventMessage::Type::PhysicalPadRelease:
        {
            //printf("Applying PhysicalPadRelease\n");
            assert(msg.physicalPadRelease);

            auto padPress = std::find_if(
                physicalPadEvents.begin(), physicalPadEvents.end(),
                [&](const auto &e)
                {
                    return e->padIndex == msg.physicalPadRelease->padIndex;
                });

            if (padPress == physicalPadEvents.end())
            {
                msg.action(nullptr);
                return;
            }

            auto padPressPtr = *padPress;
            physicalPadEvents.erase(padPress);
            publishSnapshot();
            msg.action(padPressPtr.get());
            break;
        }
        case EventMessage::Type::ProgramPadPress:
            assert(msg.programPadPress);
            //printf("Applying ProgramPadPress\n");
            programPadEvents.push_back(msg.programPadPress);
            publishSnapshot();
            msg.action(msg.programPadPress.get());
            break;

        case EventMessage::Type::ProgramPadAftertouch:
        {
            //printf("Applying ProgramPadAftertouch\n");
            for (auto &p : programPadEvents)
            {
                if (p->padIndex == msg.programPadAftertouch->padIndex &&
                        p->source == msg.source)
                {
                    p->pressure = msg.programPadAftertouch->pressure;
                    publishSnapshot();
                    msg.action(p.get());
                }
            }
            break;
        }
        case EventMessage::Type::ProgramPadRelease:
        {
            //printf("Applying ProgramPadRelease\n");
            assert(msg.programPadRelease);
            auto padPress = std::find_if(
                programPadEvents.begin(), programPadEvents.end(),
                [&](const auto &e)
                {
                    return e->padIndex == msg.programPadRelease->padIndex &&
                           e->source == msg.source &&
                           e->program == msg.programPadRelease->program;
                });


            if (padPress == programPadEvents.end())
            {
                msg.action(nullptr);
                return;
            }

            auto padPressPtr = *padPress;
            programPadEvents.erase(padPress);
            publishSnapshot();
            msg.action(padPressPtr.get());
            break;
        }
        case EventMessage::Type::NoteOn:
            assert(msg.noteOnEvent);
            noteEvents.push_back(msg.noteOnEvent);
            publishSnapshot();
            msg.action(msg.noteOnEvent.get());
            break;

        case EventMessage::Type::NoteAftertouch:
            //printf("Applying NoteAftertouch\n");
            for (auto &n : noteEvents)
            {
                if (n->noteNumber == msg.noteAftertouchEvent->noteNumber &&
                        n->source == msg.source &&
                        n->midiChannel == msg.noteAftertouchEvent->midiChannel)
                {
                    n->pressure = msg.noteAftertouchEvent->pressure;
                    publishSnapshot();
                    msg.action(n.get());
                }
            }

            break;

        case EventMessage::Type::NoteOff:
        {
            assert(msg.noteOffEvent);
            //printf("Applying NoteOff\n");
            auto noteOn = std::find_if(noteEvents.begin(), noteEvents.end(),
                                       [&](const auto &n)
                                       {
                                           return n->source == msg.source &&
                                                  n->noteNumber == msg.noteOffEvent->noteNumber &&
                                                  n->midiChannel == msg.noteOffEvent->midiChannel;
                                       });

            if (noteOn == noteEvents.end())
            {
                msg.action(nullptr);
                return;
            }

            auto noteOnPtr = *noteOn;
            noteEvents.erase(noteOn);
            publishSnapshot();
            msg.action(noteOnPtr.get());
            break;
        }
        case EventMessage::Type::Clear:
            //printf("Applying Clear\n");
            physicalPadEvents.clear();
            programPadEvents.clear();
            noteEvents.clear();
            break;
    }
}
