#include "EventRegistry.hpp"

#include "utils/TimeUtils.hpp"

#include <algorithm>

using namespace mpc::eventregistry;
using namespace mpc::concurrency;

EventRegistry::EventRegistry()
    : AtomicStateExchange(
          [&](State &s)
          {
              reserveState(s);
          })
{
}

void EventRegistry::reserveState(State &s)
{
    s.physicalPadEvents.reserve(CAPACITY);
    s.programPadEvents.reserve(CAPACITY);
    s.noteEvents.reserve(CAPACITY);
}

void EventRegistry::registerPhysicalPadPress(
    Source source, lcdgui::ScreenId screen, sequencer::BusType busType,
    PhysicalPadIndex padIndex, Velocity velocity, TrackIndex trackIndex,
    controller::Bank bank, std::optional<ProgramIndex> programIndex,
    std::optional<NoteNumber> noteNumber,
    const std::function<void(void *)> &action) const
{
    PhysicalPadPressEvent e{padIndex,
                            source,
                            screen,
                            trackIndex,
                            busType,
                            velocity,
                            bank,
                            programIndex.value_or(NoProgramIndex),
                            noteNumber.value_or(NoNoteNumber),
                            NoPressure};
    EventMessage msg{EventMessage::Type::PhysicalPadPress};
    msg.physicalPadPress = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadAftertouch(
    PhysicalPadIndex padIndex, Pressure pressure, Source source,
    const std::function<void(void *)> &action) const
{
    PhysicalPadAftertouchEvent e{padIndex, pressure};
    EventMessage msg{EventMessage::Type::PhysicalPadAftertouch};
    msg.physicalPadAftertouch = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void EventRegistry::registerPhysicalPadRelease(
    PhysicalPadIndex padIndex, Source source,
    const std::function<void(void *)> &action) const
{
    PhysicalPadReleaseEvent e{padIndex};
    EventMessage msg{EventMessage::Type::PhysicalPadRelease};
    msg.physicalPadRelease = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void EventRegistry::registerProgramPadPress(
    Source source, std::optional<MidiChannel> midiInputChannel,
    lcdgui::ScreenId screen, TrackIndex trackIndex, sequencer::BusType busType,
    ProgramPadIndex padIndex, Velocity velocity, ProgramIndex program) const
{
    ProgramPadPressEvent e{padIndex,
                           source,
                           midiInputChannel.value_or(NoMidiChannel),
                           screen,
                           trackIndex,
                           busType,
                           program,
                           velocity,
                           NoPressure,
                           utils::nowInMilliseconds()};
    EventMessage msg{EventMessage::Type::ProgramPadPress};
    msg.programPadPress = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerProgramPadAftertouch(Source source,
                                                 ProgramPadIndex padIndex,
                                                 ProgramIndex program,
                                                 Pressure pressure) const
{
    ProgramPadAftertouchEvent e{padIndex, program, pressure};
    EventMessage msg{EventMessage::Type::ProgramPadAftertouch};
    msg.programPadAftertouch = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerProgramPadRelease(
    Source source, ProgramPadIndex padIndex, ProgramIndex program,
    const std::function<void(void *)> &action) const
{
    ProgramPadReleaseEvent e{padIndex, program};
    EventMessage msg{EventMessage::Type::ProgramPadRelease};
    msg.programPadRelease = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

NoteOnEvent EventRegistry::registerNoteOn(
    Source source, std::optional<MidiChannel> midiInputChannel,
    lcdgui::ScreenId screen, TrackIndex trackIndex, sequencer::BusType busType,
    NoteNumber noteNumber, Velocity velocity,
    std::optional<ProgramIndex> programIndex,
    const std::function<void(void *)> &action) const
{
    NoteOnEvent e{noteNumber,
                  source,
                  midiInputChannel.value_or(NoMidiChannel),
                  screen,
                  trackIndex,
                  busType,
                  velocity,
                  programIndex.value_or(NoProgramIndex),
                  NoPressure};
    EventMessage msg{EventMessage::Type::NoteOn};
    msg.noteOnEvent = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
    return e;
}

void EventRegistry::registerNoteAftertouch(
    Source source, NoteNumber noteNumber, Pressure pressure,
    std::optional<MidiChannel> midiInputChannel) const
{
    NoteAftertouchEvent e{noteNumber, pressure,
                          midiInputChannel.value_or(NoMidiChannel)};
    EventMessage msg{EventMessage::Type::NoteAftertouch};
    msg.noteAftertouchEvent = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void EventRegistry::registerNoteOff(
    Source source, NoteNumber noteNumber,
    std::optional<MidiChannel> midiInputChannel,
    const std::function<void(void *)> &action) const
{
    NoteOffEvent e{
        noteNumber,
        midiInputChannel.value_or(NoMidiChannel),
    };
    EventMessage msg{EventMessage::Type::NoteOff};
    msg.noteOffEvent = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void EventRegistry::clear()
{
    enqueue({EventMessage::Type::Clear, {}, {}, {}});
}

void EventRegistry::applyMessage(const EventMessage &msg) noexcept
{
    switch (msg.type)
    {
        case EventMessage::Type::PhysicalPadPress:
            activeState.physicalPadEvents.push_back(msg.physicalPadPress);
            actions.push_back(
                [a = msg.action, e = msg.physicalPadPress]() mutable
                {
                    a(&e);
                });
            break;

        case EventMessage::Type::PhysicalPadAftertouch:
            for (auto &e : activeState.physicalPadEvents)
            {
                if (e.padIndex == msg.physicalPadAftertouch.padIndex &&
                    e.source == msg.source)
                {
                    e.pressure = msg.physicalPadAftertouch.pressure;
                    actions.push_back(
                        [a = msg.action, e]() mutable
                        {
                            a(&e);
                        });
                }
            }
            break;

        case EventMessage::Type::PhysicalPadRelease:
        {
            auto it = std::find_if(activeState.physicalPadEvents.begin(),
                                   activeState.physicalPadEvents.end(),
                                   [&](const auto &e)
                                   {
                                       return e.padIndex ==
                                              msg.physicalPadRelease.padIndex;
                                   });
            if (it == activeState.physicalPadEvents.end())
            {
                actions.push_back(
                    [a = msg.action]() mutable
                    {
                        a(nullptr);
                    });
                return;
            }
            actions.push_back(
                [a = msg.action, e = *it]() mutable
                {
                    a(&e);
                });
            activeState.physicalPadEvents.erase(it);
            break;
        }

        case EventMessage::Type::ProgramPadPress:
            activeState.programPadEvents.push_back(msg.programPadPress);
            actions.push_back(
                [a = msg.action, e = msg.programPadPress]() mutable
                {
                    a(&e);
                });
            break;

        case EventMessage::Type::ProgramPadAftertouch:
            for (auto &e : activeState.programPadEvents)
            {
                if (e.padIndex == msg.programPadAftertouch.padIndex &&
                    e.source == msg.source)
                {
                    e.pressure = msg.programPadAftertouch.pressure;
                    actions.push_back(
                        [a = msg.action, e]() mutable
                        {
                            a(&e);
                        });
                }
            }
            break;

        case EventMessage::Type::ProgramPadRelease:
        {
            auto it = std::find_if(
                activeState.programPadEvents.begin(),
                activeState.programPadEvents.end(),
                [&](const auto &e)
                {
                    return e.padIndex == msg.programPadRelease.padIndex &&
                           e.source == msg.source &&
                           e.programIndex == msg.programPadRelease.programIndex;
                });

            if (it == activeState.programPadEvents.end())
            {
                actions.push_back(
                    [a = msg.action]() mutable
                    {
                        a(nullptr);
                    });
                return;
            }

            actions.push_back(
                [a = msg.action, e = *it]() mutable
                {
                    a(&e);
                });

            activeState.programPadEvents.erase(it);
            break;
        }

        case EventMessage::Type::NoteOn:
            activeState.noteEvents.push_back(msg.noteOnEvent);
            actions.push_back(
                [a = msg.action, e = msg.noteOnEvent]() mutable
                {
                    a(&e);
                });
            break;

        case EventMessage::Type::NoteAftertouch:
            for (auto &e : activeState.noteEvents)
            {
                if (e.noteNumber == msg.noteAftertouchEvent.noteNumber &&
                    e.source == msg.source &&
                    e.midiInputChannel ==
                        msg.noteAftertouchEvent.midiInputChannel)
                {
                    e.pressure = msg.noteAftertouchEvent.pressure;
                    actions.push_back(
                        [a = msg.action, e]() mutable
                        {
                            a(&e);
                        });
                }
            }
            break;

        case EventMessage::Type::NoteOff:
        {
            auto it = std::find_if(
                activeState.noteEvents.begin(), activeState.noteEvents.end(),
                [&](const auto &n)
                {
                    return n.source == msg.source &&
                           n.noteNumber == msg.noteOffEvent.noteNumber &&
                           (n.source == Source::MidiInput
                                ? n.midiInputChannel ==
                                      msg.noteOffEvent.midiInputChannel
                                : true);
                });
            if (it == activeState.noteEvents.end())
            {
                actions.push_back(
                    [a = msg.action]() mutable
                    {
                        a(nullptr);
                    });
                return;
            }

            actions.push_back(
                [a = msg.action, e = *it]() mutable
                {
                    a(&e);
                });

            activeState.noteEvents.erase(it);
            break;
        }

        case EventMessage::Type::Clear:
            activeState.physicalPadEvents.clear();
            activeState.programPadEvents.clear();
            activeState.noteEvents.clear();
            break;
    }
}
