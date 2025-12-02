#include "PerformanceManager.hpp"

#include "utils/TimeUtils.hpp"
#include "performance/Drum.hpp"
#include "sampler/NoteParameters.hpp"

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

PerformanceManager::~PerformanceManager() {}

void PerformanceManager::reserveState(PerformanceState &s)
{
    s.physicalPadEvents.reserve(CAPACITY);
    s.programPadEvents.reserve(CAPACITY);
    s.noteEvents.reserve(CAPACITY);
}

void PerformanceManager::registerUpdateDrumProgram(
    const DrumBusIndex drumBusIndex, const ProgramIndex programIndex) const
{
    UpdateDrumProgram payload{drumBusIndex, programIndex};
    PerformanceMessage msg;
    msg.payload = std::move(payload);
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
    NoteAftertouchEvent e{noteNumber, pressure,
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
    NoteOffEvent e{noteNumber, midiInputChannel.value_or(NoMidiChannel)};

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
    std::visit(
        [&](auto &&payload)
        {
            using T = std::decay_t<decltype(payload)>;
            if constexpr (std::is_same_v<T, UpdateProgramBulk>)
            {
                activeState.programs[payload.programIndex] = payload.program;
            }
            else if constexpr (std::is_same_v<T, UpdateProgramsBulk>)
            {
                activeState.programs = payload.programs;
            }
            else if constexpr (std::is_same_v<T, SetProgramUsed>)
            {
                activeState.programs[payload.programIndex].used = true;
            }
            else if constexpr (std::is_same_v<T, RepairProgramReferences>)
            {
                for (int drumBusIndex = 0; drumBusIndex < 4; ++drumBusIndex)
                {
                    auto &drum = activeState.drums[drumBusIndex];
                    if (auto &pgm1 = drum.programIndex;
                        !activeState.programs[pgm1].used)
                    {
                        for (int pgm2 = 0;
                             pgm2 < Mpc2000XlSpecs::MAX_PROGRAM_COUNT; ++pgm2)
                        {
                            if (!activeState.programs[pgm2].used)
                            {
                                continue;
                            }

                            pgm1 = ProgramIndex(pgm2);
                            break;
                        }
                    }
                }
            }
            else if constexpr (std::is_same_v<T,
                                              UpdateProgramMidiProgramChange>)
            {
                activeState.programs[payload.programIndex].midiProgramChange =
                    payload.midiProgramChange;
            }
            else if constexpr (std::is_same_v<T, UpdateNoteParametersBulk>)
            {
                const int noteParametersIdx =
                    payload.drumNoteNumber.get() - MinDrumNoteNumber.get();
                activeState.programs[payload.programIndex]
                    .noteParameters[noteParametersIdx] = payload.noteParameters;
            }
            else if constexpr (std::is_same_v<T, DeleteSoundAndReindex>)
            {
                const int idx = payload.deletedIndex;
                for (auto &program : activeState.programs)
                {
                    for (auto &np : program.noteParameters)
                    {

                        if (np.soundIndex == idx)
                        {
                            np.soundIndex = -1;
                        }
                        else if (np.soundIndex > idx)
                        {
                            np.soundIndex--;
                        }
                    }
                }
            }
            else if constexpr (std::is_same_v<T, AddProgramSound>)
            {
                auto p = payload.programIndex;
                auto n = payload.drumNoteNumber;
                auto &srcNoteParams =
                    activeState.programs[p]
                        .noteParameters[n.get() - MinDrumNoteNumber.get()];
                auto localSoundIndex = srcNoteParams.soundIndex;

                std::string localSoundName;

                for (auto &localEntry : payload.localTable)
                {
                    if (localEntry.first == localSoundIndex)
                    {
                        localSoundName = localEntry.second;
                        break;
                    }
                }

                srcNoteParams.soundIndex = -1;

                if (!localSoundName.empty())
                {
                    for (auto &convertedEntry : payload.convertedTable)
                    {
                        if (convertedEntry.second == localSoundName)
                        {
                            srcNoteParams.soundIndex = convertedEntry.first;
                            break;
                        }
                    }
                }

                publishState();
            }
            else if constexpr (std::is_same_v<T, UpdateStereoMixer>)
            {
                StereoMixer *m;

                if (payload.drumBusIndex != NoDrumBusIndex)
                {
                    m = &activeState.drums[payload.drumBusIndex]
                             .stereoMixers[payload.drumNoteNumber.get() -
                                           MinDrumNoteNumber.get()];
                }
                else /*if (payload.programIndex != NoProgramIndex)*/
                {
                    m = &activeState.programs[payload.programIndex]
                             .noteParameters[payload.drumNoteNumber.get() -
                                             MinDrumNoteNumber.get()]
                             .stereoMixer;
                }

                m->*(payload.member) = payload.newValue;
            }
            if constexpr (std::is_same_v<T, UpdateIndivFxMixer>)
            {
                IndivFxMixer *m;

                if (payload.drumBusIndex != NoDrumBusIndex)
                {
                    m = &activeState.drums[payload.drumBusIndex]
                             .indivFxMixers[payload.drumNoteNumber.get() -
                                            MinDrumNoteNumber.get()];
                }
                else /*if (payload.programIndex != NoProgramIndex)*/
                {
                    m = &activeState.programs[payload.programIndex]
                             .noteParameters[payload.drumNoteNumber.get() -
                                             MinDrumNoteNumber.get()]
                             .indivFxMixer;
                }

                if (payload.value0To100Member)
                {
                    m->*(payload.value0To100Member) = payload.newValue;
                }
                else if (payload.individualOutputMember)
                {
                    m->*(payload.individualOutputMember) =
                        payload.individualOutput;
                }
                else if (payload.individualFxPathMember)
                {
                    m->*(payload.individualFxPathMember) =
                        payload.individualFxPath;
                }
                else /*if (payload.followStereoMember)*/
                {
                    m->*(payload.followStereoMember) = payload.followStereo;
                }
            }
            else if constexpr (std::is_same_v<T, UpdateNoteParameters>)
            {
                auto &p = activeState.programs[payload.programIndex];
                const size_t noteParametersIndex =
                    payload.drumNoteNumber.get() - MinDrumNoteNumber.get();
                auto &noteParameters = p.noteParameters[noteParametersIndex];

                if (payload.int8_tMemberToUpdate != nullptr)
                {
                    noteParameters.*(payload.int8_tMemberToUpdate) =
                        payload.int8_tValue;
                }
                else if (payload.int16_tMemberToUpdate != nullptr)
                {
                    noteParameters.*(payload.int16_tMemberToUpdate) =
                        payload.int16_tValue;
                }
                else if (payload.drumNoteMemberToUpdate != nullptr)
                {
                    noteParameters.*(payload.drumNoteMemberToUpdate) =
                        payload.drumNoteValue;
                }
                else if (payload.voiceOverlapModeMemberToUpdate != nullptr)
                {
                    noteParameters.*(payload.voiceOverlapModeMemberToUpdate) =
                        payload.voiceOverlapMode;
                }
            }
            else if constexpr (std::is_same_v<T, PhysicalPadPressEvent>)
            {
                activeState.physicalPadEvents.push_back(payload);
                actions.push_back(
                    [a = msg.action, ev = payload]() mutable
                    {
                        a(&ev);
                    });
            }
            else if constexpr (std::is_same_v<T, PhysicalPadAftertouchEvent>)
            {
                for (auto &e : activeState.physicalPadEvents)
                {
                    if (e.padIndex == payload.padIndex &&
                        e.source == msg.source)
                    {
                        e.pressure = payload.pressure;
                        actions.push_back(
                            [a = msg.action, e]() mutable
                            {
                                a(&e);
                            });
                    }
                }
            }
            else if constexpr (std::is_same_v<T, PhysicalPadReleaseEvent>)
            {
                auto it =
                    std::find_if(activeState.physicalPadEvents.begin(),
                                 activeState.physicalPadEvents.end(),
                                 [&](const auto &e)
                                 {
                                     return e.padIndex == payload.padIndex;
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
            }
            else if constexpr (std::is_same_v<T, ProgramPadPressEvent>)
            {
                activeState.programPadEvents.push_back(payload);
                actions.push_back(
                    [a = msg.action, ev = payload]() mutable
                    {
                        a(&ev);
                    });
            }
            else if constexpr (std::is_same_v<T, ProgramPadAftertouchEvent>)
            {
                for (auto &e : activeState.programPadEvents)
                {
                    if (e.padIndex == payload.padIndex &&
                        e.source == msg.source)
                    {
                        e.pressure = payload.pressure;
                        actions.push_back(
                            [a = msg.action, e]() mutable
                            {
                                a(&e);
                            });
                    }
                }
            }
            else if constexpr (std::is_same_v<T, ProgramPadReleaseEvent>)
            {
                auto it = std::find_if(
                    activeState.programPadEvents.begin(),
                    activeState.programPadEvents.end(),
                    [&](const auto &e)
                    {
                        return e.padIndex == payload.padIndex &&
                               e.source == msg.source &&
                               e.programIndex == payload.programIndex;
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
            }
            else if constexpr (std::is_same_v<T, NoteOnEvent>)
            {
                activeState.noteEvents.push_back(payload);
                actions.push_back(
                    [a = msg.action, ev = payload]() mutable
                    {
                        a(&ev);
                    });
            }
            else if constexpr (std::is_same_v<T, NoteAftertouchEvent>)
            {
                for (auto &e : activeState.noteEvents)
                {
                    if (e.noteNumber == payload.noteNumber &&
                        e.source == msg.source &&
                        e.midiInputChannel == payload.midiInputChannel)
                    {
                        e.pressure = payload.pressure;
                        actions.push_back(
                            [a = msg.action, e]() mutable
                            {
                                a(&e);
                            });
                    }
                }
            }
            else if constexpr (std::is_same_v<T, NoteOffEvent>)
            {
                auto it = std::find_if(
                    activeState.noteEvents.begin(),
                    activeState.noteEvents.end(),
                    [&](const auto &n)
                    {
                        return n.source == msg.source &&
                               n.noteNumber == payload.noteNumber &&
                               (n.source != PerformanceEventSource::MidiInput ||
                                n.midiInputChannel == payload.midiInputChannel);
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
            }
            else if constexpr (std::is_same_v<T, UpdateDrumProgram>)
            {
                activeState.drums[payload.drumBusIndex].programIndex =
                    payload.programIndex;
            }
            else if constexpr (std::is_same_v<T, std::monostate>)
            {
                activeState.physicalPadEvents.clear();
                activeState.programPadEvents.clear();
                activeState.noteEvents.clear();
            }
        },
        msg.payload);
}
