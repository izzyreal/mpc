#include "PerformanceManager.hpp"

#include "utils/TimeUtils.hpp"

#include <algorithm>

using namespace mpc::performance;
using namespace mpc::concurrency;

PerformanceManager::PerformanceManager()
    : AtomicStateExchange(
          [&](PerformanceState &s)
          {
              reserveState(s);
          })
{
}

void PerformanceManager::reserveState(PerformanceState &s) const
{
    s.physicalPadEvents.reserve(CAPACITY);
    s.programPadEvents.reserve(CAPACITY);
    s.noteEvents.reserve(CAPACITY);
}

void PerformanceManager::registerPhysicalPadPress(
    const PerformanceEventSource source, const lcdgui::ScreenId screen,
    const sequencer::BusType busType, const PhysicalPadIndex padIndex,
    const Velocity velocity, const TrackIndex trackIndex,
    const controller::Bank bank, const std::optional<ProgramIndex> programIndex,
    const std::optional<NoteNumber> noteNumber,
    const std::function<void(void *)> &action) const
{
    const PhysicalPadPressEvent e{padIndex,
                                  source,
                                  screen,
                                  trackIndex,
                                  busType,
                                  velocity,
                                  bank,
                                  programIndex.value_or(NoProgramIndex),
                                  noteNumber.value_or(NoNoteNumber),
                                  NoPressure};
    PerformanceMessage msg{PerformanceMessage::Type::PhysicalPadPress};
    msg.physicalPadPress = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadAftertouch(
    const PhysicalPadIndex padIndex, const Pressure pressure,
    const PerformanceEventSource source,
    const std::function<void(void *)> &action) const
{
    const PhysicalPadAftertouchEvent e{padIndex, pressure};
    PerformanceMessage msg{PerformanceMessage::Type::PhysicalPadAftertouch};
    msg.physicalPadAftertouch = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadRelease(
    const PhysicalPadIndex padIndex, const PerformanceEventSource source,
    const std::function<void(void *)> &action) const
{
    const PhysicalPadReleaseEvent e{padIndex};
    PerformanceMessage msg{PerformanceMessage::Type::PhysicalPadRelease};
    msg.physicalPadRelease = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadPress(
    const PerformanceEventSource source,
    const std::optional<MidiChannel> midiInputChannel,
    const lcdgui::ScreenId screen, const TrackIndex trackIndex,
    const sequencer::BusType busType, const ProgramPadIndex padIndex,
    const Velocity velocity, const ProgramIndex program,
    const PhysicalPadIndex physicalPadIndex) const
{
    const ProgramPadPressEvent e{padIndex,
                                 source,
                                 midiInputChannel.value_or(NoMidiChannel),
                                 screen,
                                 trackIndex,
                                 busType,
                                 program,
                                 velocity,
                                 NoPressure,
                                 utils::nowInMilliseconds(),
                                 physicalPadIndex};
    PerformanceMessage msg{PerformanceMessage::Type::ProgramPadPress};
    msg.programPadPress = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadAftertouch(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program, const Pressure pressure) const
{
    const ProgramPadAftertouchEvent e{padIndex, program, pressure};
    PerformanceMessage msg{PerformanceMessage::Type::ProgramPadAftertouch};
    msg.programPadAftertouch = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadRelease(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program, const std::function<void(void *)> &action) const
{
    const ProgramPadReleaseEvent e{padIndex, program};
    PerformanceMessage msg{PerformanceMessage::Type::ProgramPadRelease};
    msg.programPadRelease = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

NoteOnEvent PerformanceManager::registerNoteOn(
    const PerformanceEventSource source,
    const std::optional<MidiChannel> midiInputChannel,
    const lcdgui::ScreenId screen, const TrackIndex trackIndex,
    const sequencer::BusType busType, const NoteNumber noteNumber,
    const Velocity velocity, const std::optional<ProgramIndex> programIndex,
    const std::function<void(void *)> &action) const
{
    const NoteOnEvent e{noteNumber,
                        source,
                        midiInputChannel.value_or(NoMidiChannel),
                        screen,
                        trackIndex,
                        busType,
                        velocity,
                        programIndex.value_or(NoProgramIndex),
                        NoPressure};
    PerformanceMessage msg{PerformanceMessage::Type::NoteOn};
    msg.noteOnEvent = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
    return e;
}

void PerformanceManager::registerNoteAftertouch(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const Pressure pressure,
    const std::optional<MidiChannel> midiInputChannel) const
{
    const NoteAftertouchEvent e{noteNumber, pressure,
                                midiInputChannel.value_or(NoMidiChannel)};
    PerformanceMessage msg{PerformanceMessage::Type::NoteAftertouch};
    msg.noteAftertouchEvent = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerNoteOff(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const std::optional<MidiChannel> midiInputChannel,
    const std::function<void(void *)> &action) const
{
    const NoteOffEvent e{
        noteNumber,
        midiInputChannel.value_or(NoMidiChannel),
    };
    PerformanceMessage msg{PerformanceMessage::Type::NoteOff};
    msg.noteOffEvent = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::clear() const
{
    enqueue({PerformanceMessage::Type::Clear, {}, {}, {}});
}

void PerformanceManager::applyMessage(const PerformanceMessage &msg) noexcept
{
    switch (msg.type)
    {
        case PerformanceMessage::Type::PhysicalPadPress:
            activeState.physicalPadEvents.push_back(msg.physicalPadPress);
            actions.push_back(
                [a = msg.action, e = msg.physicalPadPress]() mutable
                {
                    a(&e);
                });
            break;

        case PerformanceMessage::Type::PhysicalPadAftertouch:
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

        case PerformanceMessage::Type::PhysicalPadRelease:
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

        case PerformanceMessage::Type::ProgramPadPress:
            activeState.programPadEvents.push_back(msg.programPadPress);
            actions.push_back(
                [a = msg.action, e = msg.programPadPress]() mutable
                {
                    a(&e);
                });
            break;

        case PerformanceMessage::Type::ProgramPadAftertouch:
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

        case PerformanceMessage::Type::ProgramPadRelease:
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

        case PerformanceMessage::Type::NoteOn:
            activeState.noteEvents.push_back(msg.noteOnEvent);
            actions.push_back(
                [a = msg.action, e = msg.noteOnEvent]() mutable
                {
                    a(&e);
                });
            break;

        case PerformanceMessage::Type::NoteAftertouch:
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

        case PerformanceMessage::Type::NoteOff:
        {
            auto it = std::find_if(
                activeState.noteEvents.begin(), activeState.noteEvents.end(),
                [&](const auto &n)
                {
                    return n.source == msg.source &&
                           n.noteNumber == msg.noteOffEvent.noteNumber &&
                           (n.source == PerformanceEventSource::MidiInput
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

        case PerformanceMessage::Type::Clear:
            activeState.physicalPadEvents.clear();
            activeState.programPadEvents.clear();
            activeState.noteEvents.clear();
            break;
    }
}
