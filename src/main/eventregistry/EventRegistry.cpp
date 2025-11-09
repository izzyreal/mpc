#include "EventRegistry.hpp"
#include "lcdgui/ScreenComponent.hpp"

#include <concurrentqueue.h>

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

    currentSnapshot = std::shared_ptr<Snapshot>(&snapA, [](Snapshot*){});
    writeTarget = &snapB;

    eventMessageQueue = std::make_shared<EventMessageQueue>(512);
}

void EventRegistry::enqueue(EventMessage &&msg) const
{
    eventMessageQueue->enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadPress(
    const Source source, const std::shared_ptr<ScreenComponent> &screen,
    const std::shared_ptr<Bus> &bus, const PhysicalPadIndex padIndex,
    const Velocity velocity, Track *track, const int bank,
    const std::optional<int> note,
    const std::function<void(void *)> &action) const
{
    // printf("registering physical pad press\n");
    assert(screen && bus);
    const auto e = std::make_shared<PhysicalPadPressEvent>();
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

void EventRegistry::registerPhysicalPadAftertouch(
    const PhysicalPadIndex padIndex, const Pressure pressure,
    const Source source, const std::function<void(void *)> &action) const
{
    // printf("registering physical pad aftertouch for pad index %i\n",
    // padIndex.get());

    const auto e = std::make_shared<PhysicalPadAftertouchEvent>(
        PhysicalPadAftertouchEvent{padIndex, pressure});
    EventMessage msg{EventMessage::Type::PhysicalPadAftertouch};
    msg.physicalPadAftertouch = e;
    msg.action = action;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadRelease(
    const PhysicalPadIndex padIndex, const Source source,
    const std::function<void(void *)> &action) const
{
    // printf("registering physical pad release\n");
    const auto e = std::make_shared<PhysicalPadReleaseEvent>(
        PhysicalPadReleaseEvent{padIndex});
    EventMessage msg;
    msg.type = EventMessage::Type::PhysicalPadRelease;
    msg.action = action;
    msg.source = source;
    msg.physicalPadRelease = e;
    enqueue(std::move(msg));
}

ProgramPadPressEventPtr EventRegistry::registerProgramPadPress(
    const Source source, const std::shared_ptr<ScreenComponent> &screen,
    const std::shared_ptr<Bus> &bus, const std::shared_ptr<Program> &program,
    const ProgramPadIndex padIndex, const Velocity velocity, Track *track,
    const std::optional<MidiChannel> midiInputChannel) const
{
    assert(screen && bus && program);
    // printf("registering program pad press with source %s\n",
    // sourceToString(source).c_str());

    auto e = std::make_shared<ProgramPadPressEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->track = track;
    e->bus = bus;
    e->program = program;
    e->velocity = velocity;
    e->midiInputChannel = midiInputChannel;
    e->pressTime = std::chrono::steady_clock::now();

    EventMessage msg{EventMessage::Type::ProgramPadPress};
    msg.programPadPress = e;
    msg.source = source;
    enqueue(std::move(msg));
    return e;
}

void EventRegistry::registerProgramPadAftertouch(
    const Source source, const std::shared_ptr<Bus> &bus,
    const std::shared_ptr<Program> &program, const ProgramPadIndex padIndex,
    const Pressure pressure, Track *track) const
{
    assert(bus && program);

    // printf("registering program pad aftertouch\n");

    const auto e = std::make_shared<ProgramPadAftertouchEvent>(
        ProgramPadAftertouchEvent{padIndex, program, pressure});
    EventMessage msg{EventMessage::Type::ProgramPadAftertouch};
    msg.programPadAftertouch = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerProgramPadRelease(
    const Source source, const std::shared_ptr<Bus> &bus,
    const std::shared_ptr<Program> &program, const ProgramPadIndex padIndex,
    Track *track, std::optional<MidiChannel> midiInputChannel,
    const std::function<void(void *)> &action) const
{
    assert(bus && program);
    // printf("registering program pad release with source %s\n",
    // sourceToString(source).c_str());

    const auto e = std::make_shared<ProgramPadReleaseEvent>(
        ProgramPadReleaseEvent{padIndex, program});
    EventMessage msg;
    msg.type = EventMessage::Type::ProgramPadRelease;
    msg.source = source;
    msg.action = action;
    msg.programPadRelease = e;
    enqueue(std::move(msg));
}

NoteOnEventPtr EventRegistry::registerNoteOn(
    const Source source, const std::shared_ptr<ScreenComponent> &screen,
    const std::shared_ptr<Bus> &bus, const NoteNumber noteNumber,
    const Velocity velocity, Track *track,
    const std::optional<MidiChannel> midiInputChannel,
    const std::shared_ptr<Program> &program,
    const std::function<void(void *)> &action) const
{
    // printf("registering note on\n");
    assert(screen && bus);
    auto e = std::make_shared<NoteOnEvent>(
        NoteOnEvent{noteNumber, source, midiInputChannel, screen, track, bus,
                    velocity, nullptr, program, std::nullopt});
    EventMessage msg{EventMessage::Type::NoteOn};
    msg.noteOnEvent = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
    return e;
}

void EventRegistry::registerNoteAftertouch(
    const Source source, const NoteNumber noteNumber, const Pressure pressure,
    const std::optional<MidiChannel> midiInputChannel) const
{
    // printf("registering note aftertouch\n");
    const auto e = std::make_shared<NoteAftertouchEvent>(
        NoteAftertouchEvent{noteNumber, pressure, midiInputChannel});
    EventMessage msg{EventMessage::Type::NoteAftertouch};
    msg.source = source;
    msg.noteAftertouchEvent = e;
    enqueue(std::move(msg));
}

void EventRegistry::registerNoteOff(
    const Source source,
    const NoteNumber noteNumber,
    const std::optional<MidiChannel> midiInputChannel,
    const std::function<void(void *)> &action) const
{
    const auto e =
        std::make_shared<NoteOffEvent>(NoteOffEvent{noteNumber, midiInputChannel});
    EventMessage msg{EventMessage::Type::NoteOff};
    msg.noteOffEvent = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void EventRegistry::clear()
{
    physicalPadEvents.clear();
    programPadEvents.clear();
    noteEvents.clear();
    enqueue({EventMessage::Type::Clear, {}, {}, {}});
}

void EventRegistry::publishSnapshotToBuffer(Snapshot *dst) const noexcept
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
    Snapshot* dst = writeTarget;
    publishSnapshotToBuffer(dst);
    auto newShared = std::shared_ptr<Snapshot>(dst, [](Snapshot*){});
    std::atomic_store(&currentSnapshot, newShared);
    writeTarget = (dst == &snapA) ? &snapB : &snapA;
}

SnapshotView EventRegistry::getSnapshot() const noexcept
{
    auto s =
        std::atomic_load_explicit(&currentSnapshot, std::memory_order_acquire);
    return SnapshotView{std::move(s)};
}

void EventRegistry::drainQueue() noexcept
{
    EventMessage msg;
    while (eventMessageQueue->try_dequeue(msg))
    {
        applyMessage(msg);
    }
}

void EventRegistry::applyMessage(const EventMessage &msg) noexcept
{
    switch (msg.type)
    {
        case EventMessage::Type::PhysicalPadPress:
            // printf("Applying PhysicalPadPress\n");
            assert(msg.physicalPadPress);
            physicalPadEvents.push_back(msg.physicalPadPress);
            publishSnapshot();
            msg.action(msg.physicalPadPress.get());
            break;

        case EventMessage::Type::PhysicalPadAftertouch:
            // printf("Applying PhysicalPadAftertouch\n");
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
            // printf("Applying PhysicalPadRelease\n");
            assert(msg.physicalPadRelease);

            const auto padPress = std::find_if(
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

            const auto padPressPtr = *padPress;
            physicalPadEvents.erase(padPress);
            publishSnapshot();
            msg.action(padPressPtr.get());
            break;
        }
        case EventMessage::Type::ProgramPadPress:
            assert(msg.programPadPress);
            // printf("Applying ProgramPadPress\n");
            programPadEvents.push_back(msg.programPadPress);
            publishSnapshot();
            msg.action(msg.programPadPress.get());
            break;

        case EventMessage::Type::ProgramPadAftertouch:
        {
            // printf("Applying ProgramPadAftertouch\n");
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
            // printf("Applying ProgramPadRelease\n");
            assert(msg.programPadRelease);
            const auto padPress = std::find_if(
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

            const auto padPressPtr = *padPress;
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
            // printf("Applying NoteAftertouch\n");
            for (auto &n : noteEvents)
            {
                if (n->noteNumber == msg.noteAftertouchEvent->noteNumber &&
                    n->source == msg.source &&
                    n->midiInputChannel == msg.noteAftertouchEvent->midiInputChannel)
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
            // printf("Applying NoteOff\n");
            const auto noteOn = std::find_if(
                noteEvents.begin(), noteEvents.end(),
                [&](const auto &n)
                {
                    const bool match =
                        n->source == msg.source &&
                        n->noteNumber == msg.noteOffEvent->noteNumber &&
                        (
                            n->source == Source::MidiInput
                                ? n->midiInputChannel == msg.noteOffEvent->midiInputChannel
                                : true
                        );                
                    return match;
                });

            if (noteOn == noteEvents.end())
            {
                msg.action(nullptr);
                return;
            }

            const auto noteOnPtr = *noteOn;
            noteEvents.erase(noteOn);
            publishSnapshot();
            msg.action(noteOnPtr.get());
            break;
        }
        case EventMessage::Type::Clear:
            // printf("Applying Clear\n");
            physicalPadEvents.clear();
            programPadEvents.clear();
            noteEvents.clear();
            break;
    }
}
