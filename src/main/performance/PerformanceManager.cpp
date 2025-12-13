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

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadAftertouch(
    const PhysicalPadIndex padIndex, const Pressure pressure,
    const PerformanceEventSource source)
{
    PhysicalPadAftertouchEvent e{padIndex, pressure};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerPhysicalPadRelease(
    const PhysicalPadIndex padIndex, const PerformanceEventSource source)
{
    PhysicalPadReleaseEvent e{padIndex};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
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

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadAftertouch(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program, const Pressure pressure)
{
    ProgramPadAftertouchEvent e{padIndex, program, pressure};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::registerProgramPadRelease(
    const PerformanceEventSource source, const ProgramPadIndex padIndex,
    const ProgramIndex program)
{
    ProgramPadReleaseEvent e{padIndex, program};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
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

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
    return e;
}

void PerformanceManager::registerNoteAftertouch(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const Pressure pressure, const std::optional<MidiChannel> midiInputChannel)
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
    const std::optional<MidiChannel> midiInputChannel)
{
    NoteOffEvent e{noteNumber, midiInputChannel.value_or(NoMidiChannel)};

    PerformanceMessage msg;
    msg.payload = e;
    msg.source = source;
    enqueue(std::move(msg));
}

void PerformanceManager::clear()
{
    PerformanceMessage msg;
    msg.payload = std::monostate{};
    enqueue(std::move(msg));
}

void PerformanceManager::applyMessage(const PerformanceMessage &msg) noexcept
{
    auto visitor = Overload{
        [&](const UpdateProgramBulk &payload)
        {
            activeState.programs[payload.programIndex] = payload.program;
        },
        [&](const SetProgramUsed &payload)
        {
            activeState.programs[payload.programIndex].used = true;
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
        [&](const UpdateProgramMidiProgramChange &payload)
        {
            activeState.programs[payload.programIndex].midiProgramChange =
                payload.midiProgramChange;
        },
        [&](const UpdateNoteParametersBulk &payload)
        {
            const int noteParametersIdx =
                payload.drumNoteNumber.get() - MinDrumNoteNumber.get();
            activeState.programs[payload.programIndex]
                .noteParameters[noteParametersIdx] = payload.noteParameters;
        },
        [&](const DeleteSoundAndReindex &payload)
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
        },
        [&](const AddProgramSound &payload)
        {
            const auto p = payload.programIndex;
            const auto n = payload.drumNoteNumber;
            auto &srcNoteParams =
                activeState.programs[p]
                    .noteParameters[n.get() - MinDrumNoteNumber.get()];
            const auto localSoundIndex = srcNoteParams.soundIndex;

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
        },
        [&](const UpdateStereoMixer &payload)
        {
            StereoMixer *m;

            if (payload.drumBusIndex == NoDrumBusIndex)
            {
                m = &activeState.programs[payload.programIndex]
                         .noteParameters[payload.drumNoteNumber.get() -
                                         MinDrumNoteNumber.get()]
                         .stereoMixer;
            }
            else
            {
                m = &activeState.drums[payload.drumBusIndex]
                         .stereoMixers[payload.drumNoteNumber.get() -
                                       MinDrumNoteNumber.get()];
            }

            m->*payload.member = payload.newValue;
        },
        [&](const UpdateIndivFxMixer &payload)
        {
            IndivFxMixer *m;

            if (payload.drumBusIndex == NoDrumBusIndex)
            {
                m = &activeState.programs[payload.programIndex]
                         .noteParameters[payload.drumNoteNumber.get() -
                                         MinDrumNoteNumber.get()]
                         .indivFxMixer;
            }
            else
            {
                m = &activeState.drums[payload.drumBusIndex]
                         .indivFxMixers[payload.drumNoteNumber.get() -
                                        MinDrumNoteNumber.get()];
            }

            if (payload.value0To100Member)
            {
                m->*payload.value0To100Member = payload.newValue;
            }
            else if (payload.individualOutputMember)
            {
                m->*payload.individualOutputMember = payload.individualOutput;
            }
            else if (payload.individualFxPathMember)
            {
                m->*payload.individualFxPathMember = payload.individualFxPath;
            }
            else /*if (payload.followStereoMember)*/
            {
                m->*payload.followStereoMember = payload.followStereo;
            }
        },
        [&](const UpdateNoteParameters &payload)
        {
            auto &p = activeState.programs[payload.programIndex];
            const size_t noteParametersIndex =
                payload.drumNoteNumber.get() - MinDrumNoteNumber.get();
            auto &noteParameters = p.noteParameters[noteParametersIndex];

            if (payload.int8_tMemberToUpdate != nullptr)
            {
                noteParameters.*payload.int8_tMemberToUpdate =
                    payload.int8_tValue;
            }
            else if (payload.int16_tMemberToUpdate != nullptr)
            {
                noteParameters.*payload.int16_tMemberToUpdate =
                    payload.int16_tValue;
            }
            else if (payload.drumNoteMemberToUpdate != nullptr)
            {
                noteParameters.*payload.drumNoteMemberToUpdate =
                    payload.drumNoteValue;
            }
            else if (payload.voiceOverlapModeMemberToUpdate != nullptr)
            {
                noteParameters.*payload.voiceOverlapModeMemberToUpdate =
                    payload.voiceOverlapMode;
            }
        },
        [&](const UpdateNoteParametersBySnapshot &payload)
        {
            auto &p = activeState.programs[payload.programIndex];
            const size_t noteParametersIndex =
                payload.drumNoteNumber.get() - MinDrumNoteNumber.get();
            auto &noteParameters = p.noteParameters[noteParametersIndex];
            noteParameters = payload.snapshot;
        },
        [&](const PhysicalPadPressEvent &payload)
        {
            activeState.physicalPadEvents.push_back(payload);
        },
        [&](const PhysicalPadAftertouchEvent &payload)
        {
            for (auto &e : activeState.physicalPadEvents)
            {
                if (e.padIndex == payload.padIndex && e.source == msg.source)
                {
                    e.pressure = payload.pressure;
                }
            }
        },
        [&](const PhysicalPadReleaseEvent &payload)
        {
            if (const auto it =
                    std::find_if(activeState.physicalPadEvents.begin(),
                                 activeState.physicalPadEvents.end(),
                                 [&](const auto &e)
                                 {
                                     return e.padIndex == payload.padIndex;
                                 });
                it != activeState.physicalPadEvents.end())
            {
                activeState.physicalPadEvents.erase(it);
            }
        },
        [&](const ProgramPadPressEvent &payload)
        {
            activeState.programPadEvents.push_back(payload);

            postToUiThread(utils::Task(
                [this, padIndex = payload.padIndex, velocity = payload.velocity,
                 source = payload.source]
                {
                    programPadEventUiCallback(padIndex, velocity, source,
                                              ProgramPadEventType::Press);
                }));
        },
        [&](const ProgramPadAftertouchEvent &payload)
        {
            for (auto &e : activeState.programPadEvents)
            {
                if (e.padIndex == payload.padIndex && e.source == msg.source)
                {
                    e.pressure = payload.pressure;
                }
            }

            postToUiThread(utils::Task(
                [this, padIndex = payload.padIndex, pressure = payload.pressure,
                 source = msg.source]
                {
                    programPadEventUiCallback(padIndex, pressure, source,
                                              ProgramPadEventType::Aftertouch);
                }));
        },
        [&](const ProgramPadReleaseEvent &payload)
        {
            if (const auto it = std::find_if(
                    activeState.programPadEvents.begin(),
                    activeState.programPadEvents.end(),
                    [&](const auto &e)
                    {
                        return e.padIndex == payload.padIndex &&
                               e.source == msg.source &&
                               e.programIndex == payload.programIndex;
                    });
                it != activeState.programPadEvents.end())
            {
                activeState.programPadEvents.erase(it);
            }

            postToUiThread(utils::Task(
                [this, padIndex = payload.padIndex, source = msg.source]
                {
                    programPadEventUiCallback(padIndex, NoVelocityOrPressure,
                                              source, ProgramPadEventType::Release);
                }));
        },
        [&](const NoteOnEvent &payload)
        {
            activeState.noteEvents.push_back(payload);
        },
        [&](const NoteAftertouchEvent &payload)
        {
            for (auto &e : activeState.noteEvents)
            {
                if (e.noteNumber == payload.noteNumber &&
                    e.source == msg.source &&
                    e.midiInputChannel == payload.midiInputChannel)
                {
                    e.pressure = payload.pressure;
                }
            }
        },
        [&](const NoteOffEvent &payload)
        {
            if (const auto it = std::find_if(
                    activeState.noteEvents.begin(),
                    activeState.noteEvents.end(),
                    [&](const auto &n)
                    {
                        return n.source == msg.source &&
                               n.noteNumber == payload.noteNumber &&
                               (n.source != PerformanceEventSource::MidiInput ||
                                n.midiInputChannel == payload.midiInputChannel);
                    });
                it != activeState.noteEvents.end())
            {
                activeState.noteEvents.erase(it);
            }
        },
        [&](const UpdateDrumProgram &payload)
        {
            activeState.drums[payload.drumBusIndex].programIndex =
                payload.programIndex;
        },
        [&](const UpdateDrumBulk &payload)
        {
            activeState.drums[payload.drum.drumBusIndex] = payload.drum;
        },
        [&](const std::monostate &)
        {
            activeState.physicalPadEvents.clear();
            activeState.programPadEvents.clear();
            activeState.noteEvents.clear();
        }};

    std::visit(visitor, msg.payload);
}
