#include "AllLoader.hpp"

#include "Mpc.hpp"

#include "StrUtil.hpp"
#include "file/ByteUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "file/kaitai/AllIo.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"

#include "disk/MpcFile.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Song.hpp"

#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MidiOutputScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimeDisplayScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "lcdgui/screens/window/LocateScreen.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/OthersScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"

#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include <kaitai/kaitaistream.h>
#include <sstream>

namespace {

std::string bytesUntilNull(const std::string& value)
{
    const auto pos = value.find('\0');
    return pos == std::string::npos ? value : value.substr(0, pos);
}

mpc::sequencer::EventData
parsedEventToEventData(const mpc2000xl_all_t::event_t& parsedEvent)
{
    mpc::sequencer::EventData e;
    e.tick = parsedEvent.tick();
    e.trackIndex = mpc::TrackIndex(parsedEvent.track());

    if (parsedEvent.note_event() != nullptr)
    {
        e.type = mpc::sequencer::EventType::NoteOn;
        e.noteNumber = mpc::NoteNumber(parsedEvent.note_event()->note());
        e.duration = mpc::Duration(parsedEvent.note_event()->duration());
        e.velocity = mpc::Velocity(parsedEvent.note_event()->velocity());
        e.noteVariationType =
            mpc::NoteVariationType(parsedEvent.note_event()->variation_type());
        e.noteVariationValue =
            mpc::NoteVariationValue(parsedEvent.note_event()->variation_value());
        return e;
    }

    if (parsedEvent.pitch_bend() != nullptr)
    {
        e.type = mpc::sequencer::EventType::PitchBend;
        auto candidate = static_cast<int>(
            parsedEvent.pitch_bend()->amount_bits_1() +
            (parsedEvent.pitch_bend()->amount_bits_2() << 8)
        ) - 16384;
        if (candidate < -8192)
        {
            candidate += 8192;
        }
        e.amount = candidate;
        return e;
    }

    if (parsedEvent.control_change() != nullptr)
    {
        e.type = mpc::sequencer::EventType::ControlChange;
        e.controllerNumber = parsedEvent.control_change()->controller();
        e.controllerValue = parsedEvent.control_change()->value();
        return e;
    }

    if (parsedEvent.program_change() != nullptr)
    {
        e.type = mpc::sequencer::EventType::ProgramChange;
        e.programChangeProgramIndex =
            mpc::ProgramIndex(parsedEvent.program_change()->program());
        return e;
    }

    if (parsedEvent.ch_pressure() != nullptr)
    {
        e.type = mpc::sequencer::EventType::ChannelPressure;
        e.amount = parsedEvent.ch_pressure()->pressure();
        return e;
    }

    if (parsedEvent.poly_pressure() != nullptr)
    {
        e.type = mpc::sequencer::EventType::PolyPressure;
        e.noteNumber = mpc::NoteNumber(parsedEvent.poly_pressure()->note());
        e.amount = parsedEvent.poly_pressure()->pressure();
        return e;
    }

    if (parsedEvent.exclusive() != nullptr)
    {
        if (parsedEvent.exclusive()->mixer() != nullptr)
        {
            e.type = mpc::sequencer::EventType::Mixer;
            auto mixerParameter =
                static_cast<int>(parsedEvent.exclusive()->mixer()->param()) - 1;
            if (mixerParameter == 4)
            {
                mixerParameter = 3;
            }
            e.mixerParameter = mixerParameter;
            e.mixerPad = parsedEvent.exclusive()->mixer()->pad_index();
            e.mixerValue = parsedEvent.exclusive()->mixer()->value();
        }
        else
        {
            e.type = mpc::sequencer::EventType::SystemExclusive;
            const auto& bytes = parsedEvent.exclusive()->bytes();
            e.sysExByteA = bytes.size() > 0
                ? static_cast<uint8_t>(bytes[0])
                : 0;
            e.sysExByteB = bytes.size() > 1
                ? static_cast<uint8_t>(bytes[1])
                : 0;
        }
    }

    return e;
}

void applyParsedSequenceToInMemorySequence(
    mpc2000xl_all_t& parsed,
    const size_t sourceIndex,
    const std::shared_ptr<mpc::sequencer::Sequence>& inMemorySequence,
    mpc::sequencer::SequencerStateManager* const manager)
{
    auto& parsedSequence = *parsed.sequences()->at(sourceIndex);
    auto* body = parsedSequence.body();
    if (body == nullptr)
    {
        return;
    }

    const auto tempoBytes = body->_unnamed2();
    const auto tempo = static_cast<double>(mpc::file::ByteUtil::bytes2ushort({tempoBytes[3], tempoBytes[4]})) / 10.0;

    inMemorySequence->init(body->bar_count() - 1);

    for (size_t i = 0; i < body->bars()->size(); ++i)
    {
        inMemorySequence->setTimeSignature(
            static_cast<int>(i),
            body->bars()->at(i)->numerator(),
            body->bars()->at(i)->denominator()
        );
    }

    const auto slotIndex = static_cast<size_t>(body->index() - 1);
    const auto* sequenceMeta = parsed.sequences_metas()->at(slotIndex).get();
    inMemorySequence->setName(mpc::StrUtil::trim(bytesUntilNull(sequenceMeta->name())));
    inMemorySequence->setInitialTempo(tempo);

    const auto sequenceIndex = inMemorySequence->getSequenceIndex();

    mpc::sequencer::UpdateSequenceTracks updateSequenceTracks{sequenceIndex};
    manager->trackStatesSnapshots[sequenceIndex] = mpc::sequencer::SequenceTrackStatesSnapshot();
    updateSequenceTracks.trackStates = &manager->trackStatesSnapshots[sequenceIndex];

    auto& trackStates = *updateSequenceTracks.trackStates;
    const auto* parsedTracks = body->tracks();

    for (int i = 0; i < mpc::Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        const auto* status = parsedTracks->status()->at(i).get();
        trackStates[i].name = bytesUntilNull(parsedTracks->names()->at(i));
        trackStates[i].deviceIndex = parsedTracks->device()->at(i);
        trackStates[i].busType = mpc::sequencer::busIndexToBusType(static_cast<int>(parsedTracks->bus()->at(i)));
        trackStates[i].programChange = parsedTracks->program_change()->at(i);
        trackStates[i].on = status->off_or_on() == mpc2000xl_all_t::OFF_ON_TRUE;
        trackStates[i].velocityRatio = parsedTracks->velocity_ratio()->at(i);
        trackStates[i].transmitProgramChangesEnabled =
            status->transmit_program_changes() == mpc2000xl_all_t::OFF_ON_TRUE;
        trackStates[i].used = status->unused_or_used() == mpc2000xl_all_t::UNUSED_USED_USED;
    }

    manager->enqueue(updateSequenceTracks);

    mpc::sequencer::UpdateSequenceEvents updateSequenceEvents{sequenceIndex};
    updateSequenceEvents.trackSnapshots = &manager->trackEventsSnapshots[sequenceIndex];
    updateSequenceEvents.trackSnapshots->clear();

    for (const auto& parsedEvent : *body->events())
    {
        if (parsedEvent->tick() >= 0xFFFFF)
        {
            continue;
        }

        auto e = parsedEventToEventData(*parsedEvent);
        if (e.type == mpc::sequencer::EventType::Unknown)
        {
            continue;
        }

        (*updateSequenceEvents.trackSnapshots)[e.trackIndex].push_back(e);
    }

    manager->enqueue(updateSequenceEvents);

    for (int i = 0; i < 32; i++)
    {
        inMemorySequence->setDeviceName(i, bytesUntilNull(body->device_names()->at(i + 1)));
    }

    inMemorySequence->setFirstLoopBarIndex(mpc::BarIndex(body->loop_start_bar_index()));
    if (body->loop_end_bar_index() > 998)
    {
        inMemorySequence->setLastLoopBarIndex(mpc::EndOfSequence);
    }
    else
    {
        inMemorySequence->setLastLoopBarIndex(mpc::BarIndex(body->loop_end_bar_index()));
    }

    inMemorySequence->setLoopEnabled(body->loop_enabled());
    inMemorySequence->getStartTime().hours = body->start_time()->hours();
    inMemorySequence->getStartTime().minutes = body->start_time()->minutes();
    inMemorySequence->getStartTime().seconds = body->start_time()->seconds();
    inMemorySequence->getStartTime().frames = body->start_time()->frames();
    inMemorySequence->getStartTime().frameDecimals = body->start_time()->frame_decimals();
}

}

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::disk;
using namespace mpc::file::all;
using namespace mpc::sequencer;

void AllLoader::loadEverythingFromFile(Mpc &mpc, MpcFile *f)
{
    file::kaitai::AllIo::loadEverything(mpc, f);
}

void AllLoader::loadEverythingFromBytes(Mpc &mpc, const std::vector<char> &bytes)
{
    const auto canonicalBytes = file::kaitai::parseRewrite<mpc2000xl_all_t>(bytes);
    loadEverythingFromCanonicalBytes(mpc, canonicalBytes);
}

void AllLoader::loadEverythingFromCanonicalBytes(
    Mpc &mpc,
    const std::vector<char> &canonicalBytes)
{
    std::stringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    auto mpcSequencer = mpc.getSequencer();

    auto* defaults = parsed.defaults();
    auto* parsedSequencer = parsed.sequencer();
    auto* songGlobal = parsed.song_global();

    auto userScreen = mpc.screens->get<ScreenId::UserScreen>();

    userScreen->setLastBar(static_cast<int>(defaults->bar_count()) - 1);
    userScreen->setLoop(defaults->unknown2()[4] == 0x01);
    userScreen->setBus(mpc::sequencer::busIndexToBusType(static_cast<int>(defaults->buses()->at(0))));

    mpcSequencer->setDefaultSequenceName(bytesUntilNull(defaults->sequence_name()));
    for (int i = 0; i < mpc::Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        mpcSequencer->setDefaultTrackName(i, bytesUntilNull(defaults->track_names()->at(i)));
    }

    userScreen->setDeviceNumber(defaults->devices()->at(0));
    userScreen->setTimeSig(defaults->numerator(), defaults->denominator());
    userScreen->setPgm(defaults->programs()->at(0));
    userScreen->setTempo(defaults->tempo() * 0.1);
    userScreen->setVelo(defaults->track_velocities()->at(0));

    mpcSequencer->deleteAllSequences();

    for (size_t sourceIndex = 0; sourceIndex < parsed.sequences()->size(); ++sourceIndex)
    {
        if (parsed.sequences()->at(sourceIndex)->body() == nullptr)
        {
            continue;
        }

        const auto targetIndex = static_cast<int>(parsed.sequences()->at(sourceIndex)->body()->index()) - 1;
        auto mpcSeq = mpcSequencer->getSequence(targetIndex);
        applyParsedSequenceToInMemorySequence(
            parsed,
            sourceIndex,
            mpcSeq,
            mpcSequencer->getStateManager().get()
        );
    }

    mpcSequencer->setSelectedSequenceIndex(
        SequenceIndex(parsedSequencer->active_sequence()), false);
    mpcSequencer->getTransport()->setPosition(0);
    mpcSequencer->setSelectedTrackIndex(parsedSequencer->active_track());
    mpcSequencer->getTransport()->setMasterTempo(
        parsedSequencer->master_tempo() * 0.1);
    mpcSequencer->getTransport()->setTempoSourceIsSequence(
        parsedSequencer->tempo_source_is_sequence());

    auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    timingCorrectScreen->setNoteValue(static_cast<int>(parsedSequencer->timing_correct()));

    auto timeDisplayScreen = mpc.screens->get<ScreenId::TimeDisplayScreen>();
    timeDisplayScreen->setDisplayStyle(
        static_cast<int>(parsedSequencer->time_display_style()));

    auto* count = parsed.count();
    auto* midiOutput = parsed.midi_output();
    auto* midiInput = parsed.midi_input();
    auto* midiSync = parsed.midi_sync();
    auto* misc = parsed.misc();
    auto* stepEditOptions = parsed.step_edit_options();

    auto countMetronomeScreen =
        mpc.screens->get<ScreenId::CountMetronomeScreen>();
    auto metronomeSoundScreen =
        mpc.screens->get<ScreenId::MetronomeSoundScreen>();

    countMetronomeScreen->setCountIn(static_cast<int>(count->count_in_mode()));
    metronomeSoundScreen->accentPad = count->accent_pad_index();
    metronomeSoundScreen->normalPad = count->normal_pad_index();
    metronomeSoundScreen->setAccentVelo(count->accent_velo());
    metronomeSoundScreen->setNormalVelo(count->normal_velo());
    metronomeSoundScreen->setOutput(static_cast<int>(count->click_output()));
    metronomeSoundScreen->setVolume(count->click_volume());
    countMetronomeScreen->setRate(static_cast<int>(count->rate()));
    metronomeSoundScreen->setSound(static_cast<int>(count->sound_source()));
    countMetronomeScreen->setInPlay(count->enabled_in_play());
    countMetronomeScreen->setInRec(count->enabled_in_rec());
    countMetronomeScreen->setWaitForKey(count->wait_for_key());
    mpcSequencer->getTransport()->setCountEnabled(count->enabled());

    mpc.screens->get<ScreenId::MidiOutputScreen>()->setSoftThru(
        static_cast<int>(midiOutput->soft_thru_mode()));

    auto midiInputScreen = mpc.screens->get<ScreenId::MidiInputScreen>();

    midiInputScreen->setReceiveCh(
        static_cast<int>(midiInput->receive_channel()) - 1);
    midiInputScreen->setType(static_cast<int>(midiInput->filter_type()));
    midiInputScreen->setMidiFilterEnabled(midiInput->filter_enabled());
    midiInputScreen->setSustainPedalToDuration(
        midiInput->sustain_pedal_to_duration());

    auto multiRecordingSetupScreen =
        mpc.screens->get<ScreenId::MultiRecordingSetupScreen>();

    for (int i = 0; i < midiInput->multi_rec_destination_tracks()->size(); i++)
    {
        multiRecordingSetupScreen->getMrsLines()[i]->setTrack(
            midiInput->multi_rec_destination_tracks()->at(i) - 1);
    }

    midiInputScreen->setChPressurePassEnabled(
        midiInput->ch_pressure_pass_enabled());
    midiInputScreen->setExclusivePassEnabled(
        midiInput->exclusive_pass_enabled());
    mpcSequencer->setRecordingModeMulti(midiInput->multi_rec_enabled());
    midiInputScreen->setNotePassEnabled(midiInput->note_pass_enabled());
    midiInputScreen->setPgmChangePassEnabled(
        midiInput->pgm_change_pass_enabled());
    midiInputScreen->setPitchBendPassEnabled(
        midiInput->pitch_bend_pass_enabled());
    midiInputScreen->setPolyPressurePassEnabled(
        midiInput->poly_pressure_pass_enabled());
    midiInputScreen->getCcPassEnabled() = *midiInput->cc_pass_enabled();

    auto stepEditOptionsScreen =
        mpc.screens->get<ScreenId::StepEditOptionsScreen>();
    stepEditOptionsScreen->setAutoStepIncrementEnabled(
        stepEditOptions->auto_step_increment());
    stepEditOptionsScreen->setDurationOfRecordedNotesTcValue(
        stepEditOptions->duration_of_recorded_notes() ==
        mpc2000xl_all_t::DURATION_OF_RECORDED_NOTES_TC_VALUE);
    stepEditOptionsScreen->setTcValueRecordedNotes(
        stepEditOptions->tc_value_percentage());
    midiInputScreen->setProgChangeSeq(parsed.prog_change_to_seq());

    auto locateScreen = mpc.screens->get<ScreenId::LocateScreen>();
    std::vector<LocateScreen::Location> locations;
    locations.reserve(parsed.locations()->size());
    for (const auto& location : *parsed.locations())
    {
        locations.emplace_back(location->bar(), location->beat(), location->clock());
    }
    locateScreen->setLocations(locations);

    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    using MT = input::midi::MidiBindingBase::MessageType;

    int bindingCounter = 0;
    for (const auto& midiSwitch : *misc->midi_switch())
    {
        const int cc = midiSwitch->controller() == 0xFF ? -1 : midiSwitch->controller();
        const auto functionIndex = static_cast<int>(midiSwitch->function());
        auto fn =
            static_cast<controller::MidiFootswitchFunction>(functionIndex);

        if (auto hw = controller::footswitchToComponentId.find(fn);
            hw != controller::footswitchToComponentId.end())
        {
            footswitchController->bindings[bindingCounter++] =
                input::midi::HardwareBinding{{-1, cc, std::nullopt,
                                              input::midi::Interaction::Press,
                                              MT::CC},
                                             {hw->second}};
        }
        else if (auto sq = controller::footswitchToSequencerCmd.find(fn);
                 sq != controller::footswitchToSequencerCmd.end())
        {
            footswitchController->bindings[bindingCounter++] =
                input::midi::SequencerBinding{{-1, cc, std::nullopt,
                                               input::midi::Interaction::Press,
                                               MT::CC},
                                              {sq->second}};
        }
        else
        {
            std::cout << "[Footswitch] Unmapped function index: "
                      << functionIndex << "\n";
        }
    }

    auto othersScreen = mpc.screens->get<ScreenId::OthersScreen>();

    othersScreen->setTapAveraging(static_cast<int>(misc->tap_averaging()) + 2);

    auto syncScreen = mpc.screens->get<ScreenId::SyncScreen>();

    syncScreen->receiveMMCEnabled = misc->midi_sync_in_receive_mmc_enabled();
    syncScreen->sendMMCEnabled = midiSync->send_mmc_enabled();
    syncScreen->in = midiSync->input();
    syncScreen->out = static_cast<int>(midiSync->output());

    if (!mpc.isPluginModeEnabled())
    {
        syncScreen->setModeIn(static_cast<int>(midiSync->in_mode()));
        syncScreen->setModeOut(static_cast<int>(midiSync->out_mode()));
    }

    syncScreen->shiftEarly = midiSync->shift_early();
    syncScreen->frameRate = static_cast<int>(midiSync->frame_rate());

    mpcSequencer->setSecondSequenceEnabled(
        parsedSequencer->second_sequence_enabled());

    auto secondSequenceScreen = mpc.screens->get<ScreenId::SecondSeqScreen>();
    secondSequenceScreen->sq = parsedSequencer->sequence_sequence_index();

    auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    songScreen->setDefaultSongName(songGlobal->default_song_name());
    auto ignoreTempoChangeScreen =
        mpc.screens->get<ScreenId::IgnoreTempoChangeScreen>();
    ignoreTempoChangeScreen->setIgnore(
        songGlobal->ignore_tempo_change_events_in_sequence());

    const auto* songs = parsed.songs();

    for (int i = 0; i < 20; i++)
    {
        const auto& parsedSong = songs->at(i);
        auto mpcSong = mpcSequencer->getSong(i);
        mpcSong->setUsed(false);

        const auto trimmedName = mpc::StrUtil::trim(bytesUntilNull(parsedSong->name()));
        const bool isUsed = parsedSong->is_used() || trimmedName != "(Unused)";
        if (isUsed)
        {
            mpcSong->setUsed(true);
            mpcSong->setName(trimmedName);

            int stepIndex = 0;
            for (const auto& step : *parsedSong->steps())
            {
                if (step->sequence_index() == 0xFF || step->repeat_count() == 0xFF)
                {
                    continue;
                }

                const auto idx = SongStepIndex(stepIndex++);
                mpcSong->insertStep(idx);
                mpcSong->setStepSequenceIndex(
                    idx, SequenceIndex(step->sequence_index()));
                mpcSong->setStepRepetitionCount(idx, step->repeat_count());
            }

            mpcSong->setFirstLoopStepIndex(
                SongStepIndex(parsedSong->loop_first_step()));

            mpcSong->setLastLoopStepIndex(
                SongStepIndex(parsedSong->loop_last_step()));

            mpcSong->setLoopEnabled(parsedSong->is_loop_enabled());
        }
    }

    mpcSequencer->getStateManager()->enqueue(
        SetSelectedSongStepIndex{SongStepIndex(0), NoSequenceIndex});
}

std::vector<SequenceMetaInfo>
AllLoader::loadSequenceMetaInfosFromFile(Mpc &mpc, MpcFile *f)
{
    auto result = file::kaitai::AllIo::loadSequenceMetaInfos(mpc, f);
    if (!result)
    {
        throw std::runtime_error(result.error());
    }
    return *result;
}

std::shared_ptr<Sequence>
AllLoader::loadOneSequenceFromCanonicalBytes(
    Mpc &mpc,
    const std::vector<char> &canonicalBytes,
    const SequenceIndex sourceIndexInAllFile,
    const SequenceIndex destIndexInMpcMemory)
{
    std::stringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    size_t sourceOrdinal = parsed.sequences()->size();
    const auto sourceSlot = static_cast<int>(sourceIndexInAllFile);
    for (size_t i = 0; i < parsed.sequences()->size(); ++i)
    {
        auto* body = parsed.sequences()->at(i)->body();
        if (body == nullptr)
        {
            continue;
        }

        if (static_cast<int>(body->index()) - 1 == sourceSlot)
        {
            sourceOrdinal = i;
            break;
        }
    }

    const auto sequence = mpc.getSequencer()->getSequence(destIndexInMpcMemory);
    if (sourceOrdinal == parsed.sequences()->size())
    {
        return sequence;
    }

    applyParsedSequenceToInMemorySequence(
        parsed,
        sourceOrdinal,
        sequence,
        mpc.getSequencer()->getStateManager().get()
    );

    return sequence;
}

std::shared_ptr<Sequence>
AllLoader::loadOneSequenceFromFile(Mpc &mpc, MpcFile *f,
                                   const SequenceIndex sourceIndexInAllFile,
                                   const SequenceIndex destIndexInMpcMemory)
{
    return file::kaitai::AllIo::loadOneSequence(
        mpc, f, sourceIndexInAllFile, destIndexInMpcMemory);
}
