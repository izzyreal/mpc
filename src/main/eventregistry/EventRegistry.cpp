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
                    if (events){
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
}

PhysicalPadEventPtr EventRegistry::registerPhysicalPadPress(
    Source source, std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus, PhysicalPadIndex padIndex,
    Velocity velocity, std::shared_ptr<sequencer::Track> track, int bank,
    std::optional<int> note)
{
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
    physicalPadEvents.push_back(e);
    return e;
}

void EventRegistry::registerPhysicalPadAftertouch(PhysicalPadIndex padIndex,
                                                  Pressure pressure)
{
    for (auto &e : physicalPadEvents)
    {
        if (e->padIndex == padIndex && e->source != Source::NoteRepeat)
        {
            e->pressure = pressure;
        }
    }
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

    auto e = std::make_shared<ProgramPadEvent>();
    e->padIndex = padIndex;
    e->source = source;
    e->screen = screen;
    e->trackIndex = trackIndex;
    e->bus = bus;
    e->program = program;
    e->velocity = velocity;
    e->midiChannel = midiChannel;

    programPadEvents.push_back(e);
    return e;
}

void EventRegistry::registerProgramPadAftertouch(
    Source source, std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program, ProgramPadIndex padIndex,
    Pressure pressure, std::optional<TrackIndex> trackIndex)
{
    assert(bus && program);

    printf("Registering pad pressure: %i\n", pressure.get());
    for (auto &e : programPadEvents)
    {
        if (e->padIndex == padIndex && e->source == source)
        {
            e->pressure = pressure;
            return;
        }
    }

    throw std::invalid_argument(
        "This method should only be called for program pads that are "
        "pressed\n");
}

NoteEventPtr EventRegistry::registerNoteOn(
    Source source, std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus, NoteNumber noteNumber,
    Velocity velocity, std::optional<TrackIndex> trackIndex,
    std::optional<MidiChannel> midiChannel)
{
    assert(screen && bus);

    auto e = std::make_shared<NoteEvent>();
    e->noteNumber = noteNumber;
    e->source = source;
    e->screen = screen;
    e->trackIndex = trackIndex;
    e->bus = bus;
    e->velocity = velocity;
    e->midiChannel = midiChannel;
    noteEvents.push_back(e);
    return e;
}

void EventRegistry::registerNoteAftertouch(Source source, NoteNumber noteNumber,
                                           Pressure pressure)
{
    printf("Registering note pressure: %i\n", pressure.get());
    for (auto &e : noteEvents)
    {
        if (e->noteNumber == noteNumber && e->source == source &&
            e->source != Source::NoteRepeat)
        {
            e->pressure = pressure;
            return;
        }
    }

    throw std::invalid_argument(
        "This method should only be called for program pads that are "
        "pressed\n");
}

bool EventRegistry::isProgramPadPressedBySource(ProgramPadIndex padIndex,
                                                Source source) const
{
    for (const auto &e : programPadEvents)
    {
        if (e->padIndex == padIndex && e->source == source)
        {
            return true;
        }
    }
    return false;
}

VelocityOrPressure EventRegistry::getPressedProgramPadAfterTouchOrVelocity(
    ProgramPadIndex padIndex) const
{
    assert(isProgramPadPressed(padIndex));

    std::optional<VelocityOrPressure> result;

    for (const auto &e : programPadEvents)
    {
        if (e->source == Source::NoteRepeat)
        {
            continue;
        }

        if (e->padIndex == padIndex)
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
        printf("returning %i\n", result.value());
        return *result;
    }

    throw std::invalid_argument(
        "This method should only be called for program pads that are "
        "pressed, and the presses shouldn't be generated by the note repeat "
        "feature.\n");
}

bool EventRegistry::isProgramPadPressed(ProgramPadIndex padIndex) const
{
    for (const auto &e : programPadEvents)
    {
        if (e->padIndex == padIndex)
        {
            return true;
        }
    }
    return false;
}

bool EventRegistry::isAnyProgramPadPressed() const
{
    return !programPadEvents.empty();
}

void EventRegistry::clear()
{
    physicalPadEvents.clear();
    programPadEvents.clear();
    noteEvents.clear();
}

NoteEventPtr EventRegistry::retrievePlayNoteEvent(NoteNumber noteNumber) const
{
    for (auto &e : noteEvents)
    {
        if (e->noteNumber == noteNumber)
        {
            return e;
        }
    }
    return {};
}

std::shared_ptr<mpc::sequencer::NoteOnEvent>
EventRegistry::retrieveRecordNoteEvent(NoteNumber noteNumber) const
{
    for (auto &e : noteEvents)
    {
        if (e->recordNoteEvent.has_value() && e->noteNumber == noteNumber)
        {
            return *e->recordNoteEvent;
        }
    }
    return {};
}

PhysicalPadEventPtr
EventRegistry::registerPhysicalPadRelease(PhysicalPadIndex padIndex)
{
    for (auto &e : physicalPadEvents)
    {
        if (e->padIndex == padIndex)
        {
            return e;
        }
    }
    return {};
}

void EventRegistry::registerNoteOff(Source source,
                                    std::shared_ptr<mpc::sequencer::Bus> bus,
                                    NoteNumber noteNumber,
                                    std::optional<TrackIndex> trackIndex,
                                    std::optional<MidiChannel> midiChannel)
{
    assert(bus);

    for (auto it = noteEvents.rbegin(); it != noteEvents.rend(); ++it)
    {
        auto &e = *it;
        if (e->noteNumber == noteNumber && e->source == source &&
            ((!midiChannel && !e->midiChannel) ||
             (midiChannel && e->midiChannel && *midiChannel == e->midiChannel)))
        {
            noteEvents.erase(std::next(it).base());
            return;
        }
    }

    throw std::invalid_argument(
        "This method should only be called for note offs for which a "
        "registered note on exists.");
}

void EventRegistry::registerProgramPadRelease(
    Source source, std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program, ProgramPadIndex padIndex,
    std::optional<TrackIndex> trackIndex,
    std::optional<MidiChannel> midiChannel)
{
    assert(bus && program);

    // Find the most recent matching press
    for (auto it = programPadEvents.rbegin(); it != programPadEvents.rend();
         ++it)
    {
        auto &e = *it;
        if (e->padIndex == padIndex && e->source == source &&
            ((!midiChannel && !e->midiChannel) ||
             (midiChannel && e->midiChannel && *midiChannel == e->midiChannel)))
        {
            programPadEvents.erase(std::next(it).base());
            return;
        }
    }

    throw std::invalid_argument(
        "This method should only be called for program pads that are pressed.");
}

PhysicalPadEventPtr
EventRegistry::retrievePhysicalPadEvent(PhysicalPadIndex padIndex) const
{
    for (auto &e : physicalPadEvents)
    {
        if (e->padIndex == padIndex)
        {
            return e;
        }
    }

    throw std::invalid_argument(
        "This method should only be called for physical pads that are "
        "pressed\n");
}

namespace
{
    template <typename EventVec, typename Pred>
    void eraseLastMatching(EventVec &vec, Pred pred)
    {
        for (auto it = vec.rbegin(); it != vec.rend(); ++it)
        {
            if (pred(*it))
            {
                vec.erase(std::next(it).base());
                break;
            }
        }
    }

    template <typename EventVec, typename Pred>
    void eraseFirstMatching(EventVec &vec, Pred pred)
    {
        auto it = std::find_if(vec.begin(), vec.end(), pred);
        if (it != vec.end())
        {
            vec.erase(it);
        }
    }
} // namespace
