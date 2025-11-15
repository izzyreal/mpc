#include "PerformanceManager.hpp"

#include "utils/TimeUtils.hpp"
#include "performance/ProgramMapper.hpp"

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

void PerformanceManager::registerSetDrumProgram(const DrumBusIndex drumBusIndex, const std::shared_ptr<sampler::Program> sp)
{
    SetDrumProgram payload{drumBusIndex};
    mapSamplerProgramToPerformanceProgram(*sp, payload.performanceProgram);
    PerformanceMessage msg;
    msg.payload = payload;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadPress(
    const PerformanceEventSource source, const lcdgui::ScreenId screen,
    const sequencer::BusType busType, const PhysicalPadIndex padIndex,
    const Velocity velocity, const TrackIndex trackIndex,
    const controller::Bank bank, const std::optional<ProgramIndex> programIndex,
    const std::optional<NoteNumber> noteNumber,
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

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadAftertouch(
    const PhysicalPadIndex padIndex, const Pressure pressure,
    const PerformanceEventSource source,
    const std::function<void(void *)> &action) const
{
    PhysicalPadAftertouchEvent e{padIndex, pressure};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadRelease(
    const PhysicalPadIndex padIndex, const PerformanceEventSource source,
    const std::function<void(void *)> &action) const
{
    PhysicalPadReleaseEvent e{padIndex};

    PerformanceMessage msg;
    msg.payload = e;
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
    ProgramPadPressEvent e{padIndex,
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

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadAftertouch(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program, const Pressure pressure) const
{
    ProgramPadAftertouchEvent e{padIndex, program, pressure};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadRelease(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program, const std::function<void(void *)> &action) const
{
    ProgramPadReleaseEvent e{padIndex, program};

    PerformanceMessage msg;
    msg.payload = e;
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
    NoteOnEvent e{noteNumber,
                  source,
                  midiInputChannel.value_or(NoMidiChannel),
                  screen,
                  trackIndex,
                  busType,
                  velocity,
                  programIndex.value_or(NoProgramIndex),
                  NoPressure};

    PerformanceMessage msg;
    msg.payload = e;
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
    NoteAftertouchEvent e{noteNumber,
                          pressure,
                          midiInputChannel.value_or(NoMidiChannel)};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerNoteOff(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const std::optional<MidiChannel> midiInputChannel,
    const std::function<void(void *)> &action) const
{
    NoteOffEvent e{noteNumber,
                   midiInputChannel.value_or(NoMidiChannel)};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    msg.action = action;
    enqueue(std::move(msg));
}

void PerformanceManager::clear() const
{
    PerformanceMessage msg;
    msg.payload = std::monostate{};
    enqueue(std::move(msg));
}

void PerformanceManager::applyMessage(const PerformanceMessage &msg) noexcept
{
    std::visit([&](auto &&ev)
    {
        using T = std::decay_t<decltype(ev)>;

        if constexpr (std::is_same_v<T, PhysicalPadPressEvent>)
        {
            activeState.physicalPadEvents.push_back(ev);
            actions.push_back([a = msg.action, ev = ev]() mutable { a(&ev); });
        }
        else if constexpr (std::is_same_v<T, PhysicalPadAftertouchEvent>)
        {
            for (auto &e : activeState.physicalPadEvents)
            {
                if (e.padIndex == ev.padIndex && e.source == msg.source)
                {
                    e.pressure = ev.pressure;
                    actions.push_back([a = msg.action, e]() mutable { a(&e); });
                }
            }
        }
        else if constexpr (std::is_same_v<T, PhysicalPadReleaseEvent>)
        {
            auto it = std::find_if(activeState.physicalPadEvents.begin(),
                                   activeState.physicalPadEvents.end(),
                                   [&](const auto &e)
                                   {
                                       return e.padIndex == ev.padIndex;
                                   });

            if (it == activeState.physicalPadEvents.end())
            {
                actions.push_back([a = msg.action]() mutable { a(nullptr); });
                return;
            }

            actions.push_back([a = msg.action, e = *it]() mutable { a(&e); });
            activeState.physicalPadEvents.erase(it);
        }
        else if constexpr (std::is_same_v<T, ProgramPadPressEvent>)
        {
            activeState.programPadEvents.push_back(ev);
            actions.push_back([a = msg.action, ev = ev]() mutable { a(&ev); });
        }
        else if constexpr (std::is_same_v<T, ProgramPadAftertouchEvent>)
        {
            for (auto &e : activeState.programPadEvents)
            {
                if (e.padIndex == ev.padIndex && e.source == msg.source)
                {
                    e.pressure = ev.pressure;
                    actions.push_back([a = msg.action, e]() mutable { a(&e); });
                }
            }
        }
        else if constexpr (std::is_same_v<T, ProgramPadReleaseEvent>)
        {
            auto it = std::find_if(activeState.programPadEvents.begin(),
                                   activeState.programPadEvents.end(),
                                   [&](const auto &e)
                                   {
                                       return e.padIndex == ev.padIndex &&
                                              e.source == msg.source &&
                                              e.programIndex == ev.programIndex;
                                   });

            if (it == activeState.programPadEvents.end())
            {
                actions.push_back([a = msg.action]() mutable { a(nullptr); });
                return;
            }

            actions.push_back([a = msg.action, e = *it]() mutable { a(&e); });
            activeState.programPadEvents.erase(it);
        }
        else if constexpr (std::is_same_v<T, NoteOnEvent>)
        {
            activeState.noteEvents.push_back(ev);
            actions.push_back([a = msg.action, ev = ev]() mutable { a(&ev); });
        }
        else if constexpr (std::is_same_v<T, NoteAftertouchEvent>)
        {
            for (auto &e : activeState.noteEvents)
            {
                if (e.noteNumber == ev.noteNumber &&
                    e.source == msg.source &&
                    e.midiInputChannel == ev.midiInputChannel)
                {
                    e.pressure = ev.pressure;
                    actions.push_back([a = msg.action, e]() mutable { a(&e); });
                }
            }
        }
        else if constexpr (std::is_same_v<T, NoteOffEvent>)
        {
            auto it = std::find_if(
                activeState.noteEvents.begin(), activeState.noteEvents.end(),
                [&](const auto &n)
                {
                    return n.source == msg.source &&
                           n.noteNumber == ev.noteNumber &&
                           (n.source != PerformanceEventSource::MidiInput ||
                            n.midiInputChannel == ev.midiInputChannel);
                });

            if (it == activeState.noteEvents.end())
            {
                actions.push_back([a = msg.action]() mutable { a(nullptr); });
                return;
            }

            actions.push_back([a = msg.action, e = *it]() mutable { a(&e); });
            activeState.noteEvents.erase(it);
        }
        else if constexpr (std::is_same_v<T, SetDrumProgram>)
        {

        }
        else if constexpr (std::is_same_v<T, std::monostate>)
        {
            activeState.physicalPadEvents.clear();
            activeState.programPadEvents.clear();
            activeState.noteEvents.clear();
        }

    }, msg.payload);
}

