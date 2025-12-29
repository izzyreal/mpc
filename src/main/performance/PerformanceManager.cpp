#include "PerformanceManager.hpp"

#include "utils/TimeUtils.hpp"
#include "performance/Drum.hpp"
#include "sampler/NoteParameters.hpp"
#include "utils/VariantUtils.hpp"

#include <algorithm>

using namespace mpc::performance;
using namespace mpc::concurrency;

PerformanceManager::PerformanceManager(
    const utils::PostToUiThreadFn &postToUiThread)
    : AtomicStateExchange(
          [&](PerformanceState &s)
          {
              reserveState(s);
          }),
      postToUiThread(postToUiThread)
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
    const DrumBusIndex drumBusIndex, const ProgramIndex programIndex)
{
    UpdateDrumProgram msg{drumBusIndex, programIndex};
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadPress(
    const PerformanceEventSource source, const lcdgui::ScreenId screen,
    const sequencer::BusType busType, const PhysicalPadIndex padIndex,
    const Velocity velocity, const TrackIndex trackIndex,
    const controller::Bank bank, const std::optional<ProgramIndex> programIndex,
    const std::optional<NoteNumber> noteNumber)
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

    enqueue(std::move(e));
}

void PerformanceManager::registerPhysicalPadAftertouch(
    const PhysicalPadIndex padIndex, const Pressure pressure,
    const PerformanceEventSource source)
{
    PhysicalPadAftertouchEvent e{padIndex, pressure, source};
    enqueue(std::move(e));
}

void PerformanceManager::registerPhysicalPadRelease(
    const PhysicalPadIndex padIndex)
{
    PhysicalPadReleaseEvent e{padIndex};
    enqueue(std::move(e));
}

void PerformanceManager::registerProgramPadPress(
    const PerformanceEventSource source,
    const std::optional<MidiChannel> midiInputChannel,
    const lcdgui::ScreenId screen, const TrackIndex trackIndex,
    const sequencer::BusType busType, const ProgramPadIndex padIndex,
    const Velocity velocity, const ProgramIndex program,
    const PhysicalPadIndex physicalPadIndex)
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
    enqueue(std::move(e));
}

void PerformanceManager::registerProgramPadAftertouch(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program, const Pressure pressure)
{
    ProgramPadAftertouchEvent e{padIndex, program, pressure, source};
    enqueue(std::move(e));
}

void PerformanceManager::registerProgramPadRelease(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program)
{
    ProgramPadReleaseEvent e{padIndex, program, source};
    enqueue(std::move(e));
}

NoteOnEvent PerformanceManager::registerNoteOn(
    const PerformanceEventSource source,
    const std::optional<MidiChannel> midiInputChannel,
    const lcdgui::ScreenId screen, const TrackIndex trackIndex,
    const sequencer::BusType busType, const NoteNumber noteNumber,
    const Velocity velocity, const std::optional<ProgramIndex> programIndex)
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
    const NoteOnEvent copy = e;
    enqueue(std::move(e));
    return copy;
}

void PerformanceManager::registerNoteAftertouch(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const Pressure pressure, const std::optional<MidiChannel> midiInputChannel)
{
    NoteAftertouchEvent e{noteNumber, pressure,
                          midiInputChannel.value_or(NoMidiChannel), source};
    enqueue(std::move(e));
}

void PerformanceManager::registerNoteOff(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const std::optional<MidiChannel> midiInputChannel)
{
    NoteOffEvent e{noteNumber, midiInputChannel.value_or(NoMidiChannel),
                   source};
    enqueue(std::move(e));
}

void PerformanceManager::clear()
{
    enqueue(std::monostate{});
}

void PerformanceManager::applyMessage(const PerformanceMessage &msg) noexcept
{
    auto visitor = Overload{
        [&](const UpdateProgramBulk &m)
        {
            activeState.programs[m.programIndex] = m.program;
        },
        [&](const SetProgramUsed &m)
        {
            activeState.programs[m.programIndex].used = true;
        },
        [&](const RepairProgramReferences &)
        {
            for (int drumBusIndex = 0; drumBusIndex < 4; ++drumBusIndex)
            {
                auto &drum = activeState.drums[drumBusIndex];
                if (auto &pgm1 = drum.programIndex;
                    !activeState.programs[pgm1].used)
                {
                    for (int pgm2 = 0; pgm2 < Mpc2000XlSpecs::MAX_PROGRAM_COUNT;
                         ++pgm2)
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
        },
        [&](const UpdateProgramMidiProgramChange &m)
        {
            activeState.programs[m.programIndex].midiProgramChange =
                m.midiProgramChange;
        },
        [&](const UpdateAllNoteParametersBulk &m)
        {
            activeState.programs[m.programIndex].noteParameters = m.snapshot;
        },
        [&](const DeleteSoundAndReindex &m)
        {
            const int idx = m.deletedIndex;
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
        },
        [&](const AddProgramSound &m)
        {
            const auto p = m.programIndex;

            for (auto &noteParams : activeState.programs[p].noteParameters)
            {
                const auto localSoundIndex = noteParams.soundIndex;

                std::string localSoundName;

                for (const auto &localEntry : *m.pgmFileSoundTable)
                {
                    if (localEntry.first == localSoundIndex)
                    {
                        localSoundName = localEntry.second;
                        break;
                    }
                }

                noteParams.soundIndex = -1;

                if (!localSoundName.empty())
                {
                    for (auto &convertedEntry : *m.samplerSoundTable)
                    {
                        if (convertedEntry.second == localSoundName)
                        {
                            noteParams.soundIndex = convertedEntry.first;
                            break;
                        }
                    }
                }
            }
        },
        [&](const UpdateStereoMixer &m)
        {
            StereoMixer *mixer;

            if (m.drumBusIndex == NoDrumBusIndex)
            {
                mixer = &activeState.programs[m.programIndex]
                             .noteParameters[m.drumNoteNumber.get() -
                                             MinDrumNoteNumber.get()]
                             .stereoMixer;
            }
            else
            {
                mixer = &activeState.drums[m.drumBusIndex]
                             .stereoMixers[m.drumNoteNumber.get() -
                                           MinDrumNoteNumber.get()];
            }

            mixer->*m.member = m.newValue;
        },
        [&](const UpdateIndivFxMixer &m)
        {
            IndivFxMixer *mixer;

            if (m.drumBusIndex == NoDrumBusIndex)
            {
                mixer = &activeState.programs[m.programIndex]
                             .noteParameters[m.drumNoteNumber.get() -
                                             MinDrumNoteNumber.get()]
                             .indivFxMixer;
            }
            else
            {
                mixer = &activeState.drums[m.drumBusIndex]
                             .indivFxMixers[m.drumNoteNumber.get() -
                                            MinDrumNoteNumber.get()];
            }

            if (m.value0To100Member)
            {
                mixer->*m.value0To100Member = m.newValue;
            }
            else if (m.individualOutputMember)
            {
                mixer->*m.individualOutputMember = m.individualOutput;
            }
            else if (m.individualFxPathMember)
            {
                mixer->*m.individualFxPathMember = m.individualFxPath;
            }
            else /*if (m.followStereoMember)*/
            {
                mixer->*m.followStereoMember = m.followStereo;
            }
        },
        [&](const UpdateNoteParameters &m)
        {
            auto &p = activeState.programs[m.programIndex];
            const size_t noteParametersIndex =
                m.drumNoteNumber.get() - MinDrumNoteNumber.get();
            auto &noteParameters = p.noteParameters[noteParametersIndex];

            if (m.int8_tMemberToUpdate != nullptr)
            {
                noteParameters.*m.int8_tMemberToUpdate = m.int8_tValue;
            }
            else if (m.int16_tMemberToUpdate != nullptr)
            {
                noteParameters.*m.int16_tMemberToUpdate = m.int16_tValue;
            }
            else if (m.drumNoteMemberToUpdate != nullptr)
            {
                noteParameters.*m.drumNoteMemberToUpdate = m.drumNoteValue;
            }
            else if (m.voiceOverlapModeMemberToUpdate != nullptr)
            {
                noteParameters.*m.voiceOverlapModeMemberToUpdate =
                    m.voiceOverlapMode;
            }
        },
        [&](const UpdateNoteParametersBySnapshot &m)
        {
            auto &p = activeState.programs[m.programIndex];
            const size_t noteParametersIndex =
                m.drumNoteNumber.get() - MinDrumNoteNumber.get();
            auto &noteParameters = p.noteParameters[noteParametersIndex];
            noteParameters = m.snapshot;
        },
        [&](const PhysicalPadPressEvent &m)
        {
            activeState.physicalPadEvents.push_back(m);
        },
        [&](const PhysicalPadAftertouchEvent &m)
        {
            for (auto &e : activeState.physicalPadEvents)
            {
                if (e.padIndex == m.padIndex && e.source == m.source)
                {
                    e.pressure = m.pressure;
                }
            }
        },
        [&](const PhysicalPadReleaseEvent &m)
        {
            if (const auto it =
                    std::find_if(activeState.physicalPadEvents.begin(),
                                 activeState.physicalPadEvents.end(),
                                 [&](const auto &e)
                                 {
                                     return e.padIndex == m.padIndex;
                                 });
                it != activeState.physicalPadEvents.end())
            {
                activeState.physicalPadEvents.erase(it);
            }
        },
        [&](const ProgramPadPressEvent &m)
        {
            activeState.programPadEvents.push_back(m);

            postToUiThread(utils::Task(
                [this, padIndex = m.padIndex, velocity = m.velocity,
                 source = m.source]
                {
                    programPadEventUiCallback(padIndex, velocity, source,
                                              ProgramPadEventType::Press);
                }));
        },
        [&](const ProgramPadAftertouchEvent &m)
        {
            bool shouldUpdateUi = false;

            for (auto &e : activeState.programPadEvents)
            {
                if (e.padIndex == m.padIndex && e.source == m.source)
                {
                    e.pressure = m.pressure;
                    shouldUpdateUi = true;
                }
            }

            if (shouldUpdateUi)
            {
                postToUiThread(utils::Task(
                    [this, padIndex = m.padIndex, pressure = m.pressure,
                     source = m.source]
                    {
                        programPadEventUiCallback(
                            padIndex, pressure, source,
                            ProgramPadEventType::Aftertouch);
                    }));
            }
        },
        [&](const ProgramPadReleaseEvent &m)
        {
            if (const auto it =
                    std::find_if(activeState.programPadEvents.begin(),
                                 activeState.programPadEvents.end(),
                                 [&](const auto &e)
                                 {
                                     return e.padIndex == m.padIndex &&
                                            e.source == m.source &&
                                            e.programIndex == m.programIndex;
                                 });
                it != activeState.programPadEvents.end())
            {
                activeState.programPadEvents.erase(it);
            }

            postToUiThread(utils::Task(
                [this, padIndex = m.padIndex, source = m.source]
                {
                    programPadEventUiCallback(padIndex, NoVelocityOrPressure,
                                              source,
                                              ProgramPadEventType::Release);
                }));
        },
        [&](const NoteOnEvent &m)
        {
            activeState.noteEvents.push_back(m);
        },
        [&](const NoteAftertouchEvent &m)
        {
            for (auto &e : activeState.noteEvents)
            {
                if (e.noteNumber == m.noteNumber && e.source == m.source &&
                    e.midiInputChannel == m.midiInputChannel)
                {
                    e.pressure = m.pressure;
                }
            }
        },
        [&](const NoteOffEvent &m)
        {
            if (const auto it = std::find_if(
                    activeState.noteEvents.begin(),
                    activeState.noteEvents.end(),
                    [&](const auto &n)
                    {
                        return n.source == m.source &&
                               n.noteNumber == m.noteNumber &&
                               (n.source != PerformanceEventSource::MidiInput ||
                                n.midiInputChannel == m.midiInputChannel);
                    });
                it != activeState.noteEvents.end())
            {
                activeState.noteEvents.erase(it);
            }
        },
        [&](const UpdateDrumProgram &m)
        {
            activeState.drums[m.drumBusIndex].programIndex = m.programIndex;
        },
        [&](const UpdateDrumBulk &m)
        {
            activeState.drums[m.drum.drumBusIndex] = m.drum;
        },
        [&](const ActivateQuantizedLock &)
        {
            for (auto &e : activeState.programPadEvents)
            {
                if (e.source == PerformanceEventSource::MidiInput ||
                    e.source == PerformanceEventSource::VirtualMpcHardware)
                {
                    e.quantizedLockActivated = true;
                }
            }
        },
        [&](const std::monostate &)
        {
            activeState.physicalPadEvents.clear();
            activeState.programPadEvents.clear();
            activeState.noteEvents.clear();
        }};

    std::visit(visitor, msg);
}
