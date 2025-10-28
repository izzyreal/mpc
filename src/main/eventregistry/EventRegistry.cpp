#include "EventRegistry.hpp"
#include "lcdgui/ScreenComponent.hpp"

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
                    if (events)
                        assert(events->source == expected);
                }
            }()),
         ...);
    }
}

EventRegistry::EventRegistry()
{
    physicalPadEvents.reserve(CAPACITY);
    programPadEvents.reserve(CAPACITY);
    noteEvents.reserve(CAPACITY);
}

PhysicalPadEventPtr EventRegistry::registerPhysicalPadPress(
    Source source,
    std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    PhysicalPadIndex padIndex,
    Velocity velocity,
    std::shared_ptr<sequencer::Track> track,
    int bank,
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

void EventRegistry::registerPhysicalPadAftertouch(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    PhysicalPadIndex padIndex,
    Pressure pressure,
    std::optional<TrackIndex> trackIndex)
{
    assert(bus);

    // TODO: Find event(s) and update pressure
}

ProgramPadEventPtr EventRegistry::registerNonMidiProgramPadPress(
    Source source,
    std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program,
    ProgramPadIndex padIndex,
    Velocity velocity,
    std::optional<TrackIndex> trackIndex)
{
    return registerMidiProgramPadPress(source, screen, bus, program, padIndex,
                                       velocity, std::nullopt, trackIndex);
}

ProgramPadEventPtr EventRegistry::registerMidiProgramPadPress(
    Source source,
    std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program,
    ProgramPadIndex padIndex,
    Velocity velocity,
    std::optional<MidiChannel> midiChannel,
    std::optional<TrackIndex> trackIndex)
{
    assert(screen && bus && program);
    assertSameSource(source);

    auto e = std::make_shared<ProgramPadEvent>();
    e->padIndex = padIndex;
    e->type = ProgramPadEvent::Type::Press;
    e->source = source;
    e->midiChannel = midiChannel;
    e->screen = screen;
    e->trackIndex = trackIndex;
    e->bus = bus;
    e->program = program;
    e->velocity = velocity;

    programPadEvents.push_back(e);
    return e;
}

void EventRegistry::registerNonMidiProgramPadAftertouch(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program,
    ProgramPadIndex padIndex,
    Pressure pressure,
    std::optional<TrackIndex> trackIndex)
{
    return registerMidiProgramPadAftertouch(source, bus, program, padIndex,
                                            pressure, std::nullopt, trackIndex);
}

void EventRegistry::registerMidiProgramPadAftertouch(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program,
    ProgramPadIndex padIndex,
    Pressure pressure,
    std::optional<MidiChannel> midiChannel,
    std::optional<TrackIndex> trackIndex)
{
    assert(bus && program);
   // TODO: Find event(s) and update pressure
}

void EventRegistry::registerNonMidiProgramPadRelease(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program,
    ProgramPadIndex padIndex,
    std::optional<TrackIndex> trackIndex)
{
    return registerMidiProgramPadRelease(source, bus, program, padIndex,
                                         std::nullopt, trackIndex);
}

NoteEventPtr EventRegistry::registerNonMidiNoteOn(
    Source source,
    std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    NoteNumber noteNumber,
    Velocity velocity,
    std::optional<TrackIndex> trackIndex)
{
    return registerMidiNoteOn(source, screen, bus, noteNumber, velocity, std::nullopt, trackIndex);
}

NoteEventPtr EventRegistry::registerMidiNoteOn(
    Source source,
    std::shared_ptr<mpc::lcdgui::ScreenComponent> screen,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    NoteNumber noteNumber,
    Velocity velocity,
    std::optional<MidiChannel> midiChannel,
    std::optional<TrackIndex> trackIndex)
{
    assert(screen && bus);

    auto e = std::make_shared<NoteEvent>();
    e->noteNumber = noteNumber;
    e->type = NoteEvent::Type::On;
    e->source = source;
    e->midiChannel = midiChannel;
    e->screen = screen;
    e->trackIndex = trackIndex;
    e->bus = bus;
    e->velocity = velocity;
    noteEvents.push_back(e);
    return e;
}

void EventRegistry::registerNonMidiNoteAftertouch(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    NoteNumber noteNumber,
    Pressure pressure,
    std::optional<TrackIndex> trackIndex)
{
    return registerMidiNoteAftertouch(source, bus, noteNumber, pressure,
                                      std::nullopt, trackIndex);
}

void EventRegistry::registerMidiNoteAftertouch(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    NoteNumber noteNumber,
    Pressure pressure,
    std::optional<MidiChannel> midiChannel,
    std::optional<TrackIndex> trackIndex)
{
    assert(bus);
    // TODO: Find event(s) and update pressure
}

void EventRegistry::registerNonMidiNoteOff(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    NoteNumber noteNumber,
    std::optional<TrackIndex> trackIndex)
{
    return registerMidiNoteOff(source, bus, noteNumber,
                               std::nullopt, trackIndex);
}

bool EventRegistry::isProgramPadPressedBySource(ProgramPadIndex padIndex, Source source) const
{
    for (const auto& e : programPadEvents)
    {
        if (e->padIndex == padIndex && e->source == source &&
            e->type == ProgramPadEvent::Type::Press)
            return true;
    }
    return false;
}

std::optional<int> EventRegistry::getPressedProgramPadAfterTouchOrVelocity(ProgramPadIndex padIndex) const
{
    std::optional<int> velocityValue;
    for (const auto& e : programPadEvents)
    {
        if (e->padIndex == padIndex)
        {
            if (e->type == ProgramPadEvent::Type::Aftertouch)
                return e->pressure;
            if (e->type == ProgramPadEvent::Type::Press)
                velocityValue = e->velocity;
        }
    }
    return velocityValue;
}

bool EventRegistry::isProgramPadPressed(ProgramPadIndex padIndex) const
{
    for (const auto& e : programPadEvents)
    {
        if (e->padIndex == padIndex &&
            e->type == ProgramPadEvent::Type::Press)
            return true;
    }
    return false;
}

bool EventRegistry::isAnyProgramPadPressed() const
{
    for (const auto& e : programPadEvents)
    {
        if (e->type == ProgramPadEvent::Type::Press)
            return true;
    }
    return false;
}

void EventRegistry::clear()
{
    physicalPadEvents.clear();
    programPadEvents.clear();
    noteEvents.clear();
}

NoteEventPtr EventRegistry::retrievePlayNoteEvent(NoteNumber noteNumber) const
{
    for (auto& e : noteEvents)
    {
        if (e->noteNumber == noteNumber)
            return e;
    }
    return {};
}

std::shared_ptr<mpc::sequencer::NoteOnEvent> EventRegistry::retrieveRecordNoteEvent(NoteNumber noteNumber) const
{
    for (auto& e : noteEvents)
    {
        if (e->recordNoteEvent.has_value() && e->noteNumber == noteNumber)
            return *e->recordNoteEvent;
    }
    return {};
}

PhysicalPadEventPtr EventRegistry::registerPhysicalPadRelease(PhysicalPadIndex padIndex)
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

void EventRegistry::registerMidiNoteOff(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    NoteNumber noteNumber,
    std::optional<MidiChannel> midiChannel,
    std::optional<TrackIndex> trackIndex)
{
    assert(bus);

    // Remove matching note-on
    for (auto it = noteEvents.rbegin(); it != noteEvents.rend(); ++it)
    {
        auto& e = *it;
        if (e->noteNumber == noteNumber &&
            e->type == NoteEvent::Type::On &&
            e->source == source)
        {
            noteEvents.erase(std::next(it).base());
            break;
        }
    }
}

void EventRegistry::registerMidiProgramPadRelease(
    Source source,
    std::shared_ptr<mpc::sequencer::Bus> bus,
    std::shared_ptr<mpc::sampler::Program> program,
    ProgramPadIndex padIndex,
    std::optional<MidiChannel> midiChannel,
    std::optional<TrackIndex> trackIndex)
{
    assert(bus && program);

    // Find the most recent matching press
    for (auto it = programPadEvents.rbegin(); it != programPadEvents.rend(); ++it)
    {
        auto& e = *it;
        if (e->padIndex == padIndex &&
            e->type == ProgramPadEvent::Type::Press &&
            e->source == source)
        {
            programPadEvents.erase(std::next(it).base());
            break;
        }
    }
}

namespace {
    template <typename EventVec, typename Pred>
    void eraseLastMatching(EventVec& vec, Pred pred)
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
    void eraseFirstMatching(EventVec& vec, Pred pred)
    {
        auto it = std::find_if(vec.begin(), vec.end(), pred);
        if (it != vec.end())
            vec.erase(it);
    }
}
