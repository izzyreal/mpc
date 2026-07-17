#include "file/kaitai/AllIo.hpp"

#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientMidiFootswitchAssignmentController.hpp"
#include "controller/MidiFootswitchFunctionMap.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "input/midi/MidiControlTargetBinding.hpp"
#include "lcdgui/screens/OthersScreen.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "lcdgui/screens/window/LocateScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MidiOutputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimeDisplayScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/EventType.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"

#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace mpc::file::kaitai;

namespace {

using all_t = mpc2000xl_all_t;
constexpr size_t kVisibleNameLength = 16;
constexpr size_t kSongStepCount = 250;
constexpr size_t kMultiRecTrackCount = 34;
constexpr size_t kSequenceCount = 99;
constexpr size_t kSongCount = 20;
constexpr char kAllUnknown10[] = {0, 12, 0, 0, 0, 0, 0, 0, 0, 0};
constexpr size_t kSequenceHeaderLength = 10240;
constexpr size_t kSequenceTrackChunkLength = 1764;
constexpr size_t kSequenceBarChunkOffset = 5379;
constexpr size_t kSequenceBarChunkLength = 3996;
constexpr size_t kSequenceEventsOffset = 10240;
constexpr size_t kSequenceEventSegmentLength = 8;
constexpr char kSequencePadding1[] = {1, 1, 0};
constexpr char kSequencePadding2[] = {4, 4};
constexpr char kSequenceTrackPadding[] = {
    static_cast<char>(232), 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, static_cast<char>(232), 3
};
constexpr char kSequencePadding4[] = {40, 0, static_cast<char>(128), 0, 0};
constexpr char kSequenceTerminator[] = {
    static_cast<char>(0xFF), static_cast<char>(0xFF), static_cast<char>(0xFF), static_cast<char>(0xFF),
    static_cast<char>(0xFF), static_cast<char>(0xFF), static_cast<char>(0xFF), static_cast<char>(0xFF)
};
constexpr char kDefaultsUnknown1[] = {1, 0, 0, 1, 1, 0};
constexpr char kDefaultsUnknown2[] = {
    0, 0, static_cast<char>(0xFF), static_cast<char>(0xFF), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 32, 32, 32, 32, 32, 32, 32
};

std::vector<char> slice(const std::vector<char>& bytes, const size_t offset, const size_t length)
{
    return std::vector<char>(bytes.begin() + static_cast<std::ptrdiff_t>(offset),
                             bytes.begin() + static_cast<std::ptrdiff_t>(offset + length));
}

template <typename T>
std::unique_ptr<T> parseSection(const std::vector<char>& bytes, all_t& root)
{
    std::istringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::binary
    );
    ::kaitai::kstream parseIo(&parseStream);
    auto parsed = std::make_unique<T>(&parseIo, &root, &root);
    parsed->_read();
    return parsed;
}

template <typename T>
std::vector<char> writeStruct(T& value)
{
    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    ::kaitai::kstream writeIo(&writeStream);
    value._set_io(&writeIo);
    value._check();
    value._write();

    const auto written = writeStream.str();
    return std::vector<char>(written.begin(), written.end());
}

std::string padRight(const std::string& value, const size_t width, const char fill = ' ')
{
    if (value.size() >= width)
    {
        return value.substr(0, width);
    }
    return value + std::string(width - value.size(), fill);
}

std::unique_ptr<all_t::defaults_t> makeDefaults(mpc::Mpc& mpc, all_t& root)
{
    constexpr size_t kDefaultsLength = 1792;
    constexpr size_t kDefaultSeqNameOffset = 0;
    constexpr size_t kDefaultsUnknown1Offset = 16;
    constexpr size_t kTempoOffset = 22;
    constexpr size_t kNumeratorOffset = 24;
    constexpr size_t kDenominatorOffset = 25;
    constexpr size_t kBarCountOffset = 26;
    constexpr size_t kTickCountOffset = 28;
    constexpr size_t kUnknownNumberOffset = 32;
    constexpr size_t kDefaultsUnknown2Offset = 48;
    constexpr size_t kLoopEnabledOffset = 52;
constexpr size_t kDeviceNamesOffset = 120;
constexpr size_t kDefaultsUnknown2Length = 68;
    constexpr size_t kTrackNamesOffset = 384;
    constexpr size_t kDevicesOffset = 1408;
    constexpr size_t kBusesOffset = 1472;
    constexpr size_t kProgramsOffset = 1536;
    constexpr size_t kTrackVelocitiesOffset = 1600;
    constexpr size_t kTrackStatusesOffset = 1664;
    constexpr size_t kDefaultDeviceNameLength = 8;

    auto bytes = std::vector<char>(kDefaultsLength, char{0});
    const auto sequencer = mpc.getSequencer();
    const auto userScreen = mpc.screens->get<mpc::lcdgui::ScreenId::UserScreen>();

    const auto sequenceName = padRight(sequencer->getDefaultSequenceName(), kVisibleNameLength);
    std::copy(sequenceName.begin(), sequenceName.end(), bytes.begin() + static_cast<std::ptrdiff_t>(kDefaultSeqNameOffset));

    std::copy(std::begin(kDefaultsUnknown1), std::end(kDefaultsUnknown1), bytes.begin() + static_cast<std::ptrdiff_t>(kDefaultsUnknown1Offset));

    const auto tempo = static_cast<uint16_t>(userScreen->getTempo() * 10.0);
    bytes[kTempoOffset] = static_cast<char>(tempo & 0xFF);
    bytes[kTempoOffset + 1] = static_cast<char>((tempo >> 8) & 0xFF);
    bytes[kNumeratorOffset] = static_cast<char>(userScreen->getTimeSig().numerator);
    bytes[kDenominatorOffset] = static_cast<char>(userScreen->getTimeSig().denominator);

    const auto barCount = static_cast<uint16_t>(userScreen->getLastBar() + 1);
    bytes[kBarCountOffset] = static_cast<char>(barCount & 0xFF);
    bytes[kBarCountOffset + 1] = static_cast<char>((barCount >> 8) & 0xFF);

    auto lastBar = userScreen->getLastBar();
    uint16_t tickCount = static_cast<uint16_t>((lastBar + 1) * 384);
    if (lastBar == 1)
    {
        tickCount = 0;
        lastBar = 0;
    }
    bytes[kTickCountOffset] = static_cast<char>(tickCount & 0xFF);
    bytes[kTickCountOffset + 1] = static_cast<char>((tickCount >> 8) & 0xFF);

    const auto unknownNumber = static_cast<uint32_t>((lastBar + 1) * 2000000);
    for (int i = 0; i < 4; ++i)
    {
        bytes[kUnknownNumberOffset + i * 4] = static_cast<char>(unknownNumber & 0xFF);
        bytes[kUnknownNumberOffset + i * 4 + 1] = static_cast<char>((unknownNumber >> 8) & 0xFF);
        bytes[kUnknownNumberOffset + i * 4 + 2] = static_cast<char>((unknownNumber >> 16) & 0xFF);
        bytes[kUnknownNumberOffset + i * 4 + 3] = static_cast<char>((unknownNumber >> 24) & 0xFF);
    }

    std::copy(
        std::begin(kDefaultsUnknown2),
        std::begin(kDefaultsUnknown2) + static_cast<std::ptrdiff_t>(kDefaultsUnknown2Length),
        bytes.begin() + static_cast<std::ptrdiff_t>(kDefaultsUnknown2Offset)
    );
    bytes[kLoopEnabledOffset] = static_cast<char>(userScreen->isLoopEnabled() ? 0x01 : 0x00);

    for (int i = 0; i < 33; ++i)
    {
        const auto deviceName = i == 0
            ? std::string(kDefaultDeviceNameLength, ' ')
            : std::string("Device") + (i < 10 ? std::string("0") : std::string()) + std::to_string(i);
        std::copy(
            deviceName.begin(),
            deviceName.end(),
            bytes.begin() + static_cast<std::ptrdiff_t>(kDeviceNamesOffset + i * kDefaultDeviceNameLength)
        );
    }

    for (int i = 0; i < 64; ++i)
    {
        const auto trackName = padRight(sequencer->getDefaultTrackName(i), kVisibleNameLength);
        std::copy(
            trackName.begin(),
            trackName.end(),
            bytes.begin() + static_cast<std::ptrdiff_t>(kTrackNamesOffset + i * kVisibleNameLength)
        );
        bytes[kDevicesOffset + i] = static_cast<char>(userScreen->getDevice());
        bytes[kBusesOffset + i] = static_cast<char>(mpc::sequencer::busTypeToIndex(userScreen->getBusType()));
        bytes[kProgramsOffset + i] = static_cast<char>(userScreen->getPgm());
        bytes[kTrackVelocitiesOffset + i] = static_cast<char>(userScreen->getVelo());
        bytes[kTrackStatusesOffset + i] = static_cast<char>(userScreen->getTrackStatus());
    }

    return parseSection<all_t::defaults_t>(bytes, root);
}

std::unique_ptr<all_t::sequencer_t> makeSequencer(mpc::Mpc& mpc, all_t& root)
{
    auto sequencer = std::make_unique<all_t::sequencer_t>(nullptr, &root, &root);
    const auto mpcSequencer = mpc.getSequencer();
    const auto transport = mpcSequencer->getTransport();
    auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::ScreenId::TimingCorrectScreen>();
    auto timeDisplayScreen = mpc.screens->get<mpc::lcdgui::ScreenId::TimeDisplayScreen>();
    auto secondSeqScreen = mpc.screens->get<mpc::lcdgui::ScreenId::SecondSeqScreen>();

    sequencer->set_active_sequence(mpcSequencer->getSelectedSequenceIndex());
    sequencer->set__unnamed1(std::string(1, char{0}));
    sequencer->set_active_track(mpcSequencer->getSelectedTrackIndex());
    sequencer->set__unnamed3(std::string(1, char{0}));
    sequencer->set_master_tempo(static_cast<uint16_t>(transport->getMasterTempo() * 10.0));
    sequencer->set_tempo_source_is_sequence(transport->isTempoSourceSequence());
    sequencer->set__unnamed6(0);
    sequencer->set_timing_correct(static_cast<all_t::timing_correct_t>(timingCorrectScreen->getNoteValue()));
    sequencer->set_time_display_style(static_cast<all_t::time_display_style_t>(timeDisplayScreen->getDisplayStyle()));
    sequencer->set__unnamed9(0);
    sequencer->set_second_sequence_enabled(mpcSequencer->isSecondSequenceEnabled());
    sequencer->set__unnamed11(0);
    sequencer->set_sequence_sequence_index(secondSeqScreen->getSq());
    return sequencer;
}

std::unique_ptr<all_t::count_t> makeCount(mpc::Mpc& mpc, all_t& root)
{
    auto count = std::make_unique<all_t::count_t>(nullptr, &root, &root);
    const auto transport = mpc.getSequencer()->getTransport();
    auto countMetronomeScreen = mpc.screens->get<mpc::lcdgui::ScreenId::CountMetronomeScreen>();
    auto metronomeSoundScreen = mpc.screens->get<mpc::lcdgui::ScreenId::MetronomeSoundScreen>();

    count->set_enabled(transport->isCountEnabled());
    count->set__unnamed1(0);
    count->set_count_in_mode(static_cast<all_t::count_in_mode_t>(countMetronomeScreen->getCountInMode()));
    count->set_click_volume(static_cast<uint8_t>(metronomeSoundScreen->getVolume()));
    count->set_rate(static_cast<all_t::rate_t>(countMetronomeScreen->getRate()));
    count->set_enabled_in_play(countMetronomeScreen->getInPlay());
    count->set__unnamed6(0);
    count->set_enabled_in_rec(countMetronomeScreen->getInRec());
    count->set__unnamed8(0);
    count->set_click_output(static_cast<all_t::click_output_t>(metronomeSoundScreen->getOutput()));
    count->set_wait_for_key(countMetronomeScreen->isWaitForKeyEnabled());
    count->set__unnamed11(0);
    count->set_sound_source(static_cast<all_t::sound_source_t>(metronomeSoundScreen->getSound()));
    count->set_accent_pad_index(static_cast<uint8_t>(metronomeSoundScreen->getAccentPad()));
    count->set_normal_pad_index(static_cast<uint8_t>(metronomeSoundScreen->getNormalPad()));
    count->set_accent_velo(static_cast<uint8_t>(metronomeSoundScreen->getAccentVelo()));
    count->set_normal_velo(static_cast<uint8_t>(metronomeSoundScreen->getNormalVelo()));
    return count;
}

std::unique_ptr<all_t::midi_output_t> makeMidiOutput(mpc::Mpc& mpc, all_t& root)
{
    auto midiOutput = std::make_unique<all_t::midi_output_t>(nullptr, &root, &root);
    auto midiOutputScreen = mpc.screens->get<mpc::lcdgui::ScreenId::MidiOutputScreen>();
    midiOutput->set_soft_thru_mode(static_cast<all_t::soft_thru_mode_t>(midiOutputScreen->getSoftThru()));
    return midiOutput;
}

std::unique_ptr<all_t::midi_input_t> makeMidiInput(mpc::Mpc& mpc, all_t& root)
{
    auto midiInput = std::make_unique<all_t::midi_input_t>(nullptr, &root, &root);
    auto midiInputScreen = mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    auto multiRecordingSetupScreen = mpc.screens->get<mpc::lcdgui::ScreenId::MultiRecordingSetupScreen>();

    midiInput->set_receive_channel(static_cast<all_t::midi_input_receive_channel_t>(midiInputScreen->getReceiveCh() + 1));
    midiInput->set_sustain_pedal_to_duration(midiInputScreen->isSustainPedalToDurationEnabled());
    midiInput->set__unnamed2(0);
    midiInput->set_filter_enabled(midiInputScreen->isMidiFilterEnabled());
    midiInput->set__unnamed4(0);
    midiInput->set_filter_type(static_cast<all_t::midi_input_filter_type_t>(midiInputScreen->getType()));
    midiInput->set_multi_rec_enabled(mpc.getSequencer()->isRecordingModeMulti());
    midiInput->set__unnamed7(0);

    auto multiRecTracks = std::make_unique<std::vector<uint8_t>>();
    multiRecTracks->reserve(kMultiRecTrackCount);
    for (auto* line : multiRecordingSetupScreen->getMrsLines())
    {
        multiRecTracks->push_back(static_cast<uint8_t>(line->getTrack() + 1));
    }
    midiInput->set_multi_rec_destination_tracks(std::move(multiRecTracks));

    midiInput->set_note_pass_enabled(midiInputScreen->isNotePassEnabled());
    midiInput->set__unnamed10(0);
    midiInput->set_pitch_bend_pass_enabled(midiInputScreen->isPitchBendPassEnabled());
    midiInput->set__unnamed12(0);
    midiInput->set_pgm_change_pass_enabled(midiInputScreen->isPgmChangePassEnabled());
    midiInput->set__unnamed14(0);
    midiInput->set_ch_pressure_pass_enabled(midiInputScreen->isChPressurePassEnabled());
    midiInput->set__unnamed16(0);
    midiInput->set_poly_pressure_pass_enabled(midiInputScreen->isPolyPressurePassEnabled());
    midiInput->set__unnamed18(0);
    midiInput->set_exclusive_pass_enabled(midiInputScreen->isExclusivePassEnabled());
    midiInput->set__unnamed20(0);

    auto ccPass = std::make_unique<std::vector<bool>>(midiInputScreen->getCcPassEnabled());
    midiInput->set_cc_pass_enabled(std::move(ccPass));
    return midiInput;
}

std::unique_ptr<all_t::midi_sync_t> makeMidiSync(mpc::Mpc& mpc, all_t& root)
{
    auto midiSync = std::make_unique<all_t::midi_sync_t>(nullptr, &root, &root);
    auto syncScreen = mpc.screens->get<mpc::lcdgui::ScreenId::SyncScreen>();
    midiSync->set_in_mode(static_cast<all_t::midi_sync_mode_t>(syncScreen->getModeIn()));
    midiSync->set_out_mode(static_cast<all_t::midi_sync_mode_t>(syncScreen->getModeOut()));
    midiSync->set_shift_early(static_cast<uint8_t>(syncScreen->getShiftEarly()));
    midiSync->set_send_mmc_enabled(syncScreen->isSendMMCEnabled());
    midiSync->set__unnamed4(0);
    midiSync->set_frame_rate(static_cast<all_t::frame_rate_t>(syncScreen->getFrameRate()));
    midiSync->set_input(static_cast<uint8_t>(syncScreen->getInput()));
    midiSync->set_output(static_cast<all_t::midi_sync_output_t>(syncScreen->getOut()));
    return midiSync;
}

std::unique_ptr<all_t::song_global_t> makeSongGlobal(mpc::Mpc& mpc, all_t& root)
{
    auto songGlobal = std::make_unique<all_t::song_global_t>(nullptr, &root, &root);
    auto songScreen = mpc.screens->get<mpc::lcdgui::ScreenId::SongScreen>();
    auto ignoreTempoChangeScreen = mpc.screens->get<mpc::lcdgui::ScreenId::IgnoreTempoChangeScreen>();
    songGlobal->set_default_song_name(padRight(songScreen->getDefaultSongName(), 16));
    songGlobal->set_ignore_tempo_change_events_in_sequence(ignoreTempoChangeScreen->getIgnore());
    songGlobal->set__unnamed2(0);
    return songGlobal;
}

std::unique_ptr<std::vector<std::unique_ptr<all_t::location_t>>> makeLocations(mpc::Mpc& mpc, all_t& root)
{
    auto locateScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LocateScreen>();
    auto result = std::make_unique<std::vector<std::unique_ptr<all_t::location_t>>>();
    result->reserve(locateScreen->getLocations().size());

    for (const auto& location : locateScreen->getLocations())
    {
        auto parsedLocation = std::make_unique<all_t::location_t>(nullptr, &root, &root);
        parsedLocation->set_bar(std::get<0>(location));
        parsedLocation->set_beat(std::get<1>(location));
        parsedLocation->set_clock(std::get<2>(location));
        result->push_back(std::move(parsedLocation));
    }

    return result;
}

std::unique_ptr<all_t::misc_t> makeMisc(mpc::Mpc& mpc, all_t& root)
{
    auto misc = std::make_unique<all_t::misc_t>(nullptr, &root, &root);
    auto othersScreen = mpc.screens->get<mpc::lcdgui::ScreenId::OthersScreen>();
    auto syncScreen = mpc.screens->get<mpc::lcdgui::ScreenId::SyncScreen>();
    auto footswitchController =
        mpc.clientEventController->getClientMidiEventController()
            ->getFootswitchAssignmentController();

    misc->set_tap_averaging(static_cast<all_t::tap_averaging_t>(othersScreen->getTapAveraging() - 2));
    misc->set_midi_sync_in_receive_mmc_enabled(syncScreen->isReceiveMMCEnabled());

    auto midiSwitches = std::make_unique<std::vector<std::unique_ptr<all_t::midi_switch_t>>>();
    midiSwitches->reserve(footswitchController->bindings.size());
    for (const auto& bindingVariant : footswitchController->bindings)
    {
        auto midiSwitch = std::make_unique<all_t::midi_switch_t>(nullptr, misc.get(), &root);
        std::visit(
            [&](const auto& binding)
            {
                midiSwitch->set_controller(static_cast<uint8_t>(binding.number < 0 ? 0xFF : binding.number));
                if constexpr (std::is_same_v<std::decay_t<decltype(binding)>, mpc::input::midi::HardwareBinding>)
                {
                    const auto fn = mpc::controller::componentIdToFootswitch(binding.target.componentId).value();
                    midiSwitch->set_function(static_cast<all_t::midi_switch_function_t>(fn));
                }
                else
                {
                    const auto fn = mpc::controller::sequencerCmdToFootswitch(binding.target.command).value();
                    midiSwitch->set_function(static_cast<all_t::midi_switch_function_t>(fn));
                }
            },
            bindingVariant
        );
        midiSwitches->push_back(std::move(midiSwitch));
    }
    misc->set_midi_switch(std::move(midiSwitches));
    return misc;
}

std::unique_ptr<all_t::step_edit_options_t> makeStepEditOptions(mpc::Mpc& mpc, all_t& root)
{
    auto stepEditOptions = std::make_unique<all_t::step_edit_options_t>(nullptr, &root, &root);
    auto screen = mpc.screens->get<mpc::lcdgui::ScreenId::StepEditOptionsScreen>();
    stepEditOptions->set_auto_step_increment(screen->isAutoStepIncrementEnabled());
    stepEditOptions->set_duration_of_recorded_notes(
        screen->isDurationOfRecordedNotesTcValue()
            ? all_t::DURATION_OF_RECORDED_NOTES_TC_VALUE
            : all_t::DURATION_OF_RECORDED_NOTES_AS_PLAYED
    );
    stepEditOptions->set_tc_value_percentage(static_cast<uint8_t>(screen->getTcValuePercentage()));
    return stepEditOptions;
}

int countSequenceMetaSegments(mpc::sequencer::Sequence& seq);
int countSequenceBodySegments(mpc::sequencer::Sequence& seq);

std::unique_ptr<std::vector<std::unique_ptr<all_t::sequence_meta_t>>> makeSequenceMetas(mpc::Mpc& mpc, all_t& root)
{
    auto result = std::make_unique<std::vector<std::unique_ptr<all_t::sequence_meta_t>>>();
    const auto sequencer = mpc.getSequencer();
    result->reserve(kSequenceCount);

    for (size_t i = 0; i < kSequenceCount; ++i)
    {
        auto meta = std::make_unique<all_t::sequence_meta_t>(nullptr, &root, &root);
        const auto seq = sequencer->getSequence(static_cast<int>(i));
        meta->set_name(padRight(seq->getName(), kVisibleNameLength));
        auto segmentCount = countSequenceMetaSegments(*seq);
        if ((segmentCount & 1) != 0)
        {
            segmentCount--;
        }
        auto lastEventIndex = 0;
        if (seq->isUsed())
        {
            lastEventIndex = 641 + std::max(0, segmentCount / 2);
        }
        meta->set_last_event_index(static_cast<uint16_t>(lastEventIndex));
        result->push_back(std::move(meta));
    }

    return result;
}

std::unique_ptr<all_t::song_step_t> makeSongStep(uint8_t sequenceIndex, uint8_t repeatCount, all_t::song_t* parent, all_t& root)
{
    auto step = std::make_unique<all_t::song_step_t>(nullptr, parent, &root);
    step->set_sequence_index(sequenceIndex);
    step->set_repeat_count(repeatCount);
    return step;
}

std::unique_ptr<std::vector<std::unique_ptr<all_t::song_t>>> makeSongs(mpc::Mpc& mpc, all_t& root)
{
    auto result = std::make_unique<std::vector<std::unique_ptr<all_t::song_t>>>();
    auto sequencer = mpc.getSequencer();
    result->reserve(kSongCount);

    for (size_t i = 0; i < kSongCount; ++i)
    {
        auto song = std::make_unique<all_t::song_t>(nullptr, &root, &root);
        const auto mpcSong = sequencer->getSong(static_cast<int>(i));
        song->set_name(padRight(mpcSong->getName(), kVisibleNameLength));

        auto steps = std::make_unique<std::vector<std::unique_ptr<all_t::song_step_t>>>();
        steps->reserve(kSongStepCount);
        for (size_t stepIndex = 0; stepIndex < kSongStepCount; ++stepIndex)
        {
            if (stepIndex < mpcSong->getStepCount())
            {
                const auto stepState = mpcSong->getStep(mpc::SongStepIndex(static_cast<int>(stepIndex)));
                steps->push_back(makeSongStep(stepState.sequenceIndex, stepState.repetitionCount, song.get(), root));
            }
            else
            {
                steps->push_back(makeSongStep(0xFF, 0xFF, song.get(), root));
            }
        }
        song->set_steps(std::move(steps));
        song->set__unnamed2(std::string(2, static_cast<char>(0xFF)));
        song->set_is_used(mpcSong->isUsed());
        song->set_loop_first_step(static_cast<uint8_t>(mpcSong->getFirstLoopStepIndex()));
        song->set_loop_last_step(static_cast<uint8_t>(mpcSong->getLastLoopStepIndex()));
        song->set_is_loop_enabled(mpcSong->isLoopEnabled());
        song->set__unnamed7(std::string(6, char{0}));
        result->push_back(std::move(song));
    }

    return result;
}

int countSequenceMetaSegments(mpc::sequencer::Sequence& seq)
{
    int segmentCount = 0;
    for (const auto& track : seq.getTracks())
    {
        if (track->getIndex() > 63)
        {
            break;
        }

        for (const auto& event : track->getEvents())
        {
            switch (event->handle->type)
            {
                case mpc::sequencer::EventType::SystemExclusive:
                    segmentCount += 2;
                    break;
                case mpc::sequencer::EventType::Mixer:
                    segmentCount += 2;
                    break;
                default:
                    segmentCount += 1;
                    break;
            }
        }
    }
    return segmentCount;
}

int countSequenceBodySegments(mpc::sequencer::Sequence& seq)
{
    int segmentCount = 0;
    for (const auto& track : seq.getTracks())
    {
        if (track->getIndex() > 63)
        {
            break;
        }

        for (const auto& event : track->getEvents())
        {
            switch (event->handle->type)
            {
                case mpc::sequencer::EventType::SystemExclusive:
                    segmentCount += 3;
                    break;
                case mpc::sequencer::EventType::Mixer:
                    segmentCount += 4;
                    break;
                default:
                    segmentCount += 1;
                    break;
            }
        }
    }
    return segmentCount;
}

std::vector<char> makeTrackChunk(mpc::sequencer::Sequence& seq)
{
    std::vector<char> bytes(kSequenceTrackChunkLength, char{0});
    for (int i = 0; i < 64; ++i)
    {
        const auto track = seq.getTrack(i);
        const auto trackName = padRight(track->getName(), kVisibleNameLength);
        std::copy(trackName.begin(), trackName.end(), bytes.begin() + i * kVisibleNameLength);
        bytes[1024 + i] = static_cast<char>(track->getDeviceIndex());
        bytes[1088 + i] = static_cast<char>(mpc::sequencer::busTypeToIndex(track->getBusType()));
        bytes[1152 + i] = static_cast<char>(track->getProgramChange());
        bytes[1216 + i] = static_cast<char>(track->getVelocityRatio());

        uint8_t status = 0;
        if (track->isUsed())
        {
            status |= 1u << 0;
        }
        if (track->isOn())
        {
            status |= 1u << 1;
        }
        if (track->isTransmitProgramChangesEnabled())
        {
            status |= 1u << 2;
        }
        bytes[1280 + i] = static_cast<char>(status);
    }

    std::copy(std::begin(kSequenceTrackPadding), std::end(kSequenceTrackPadding), bytes.begin() + 1408);

    const auto lastTick = static_cast<uint32_t>(seq.getLastTick());
    const auto remainder = static_cast<uint16_t>(lastTick % 65535);
    const auto large = static_cast<uint8_t>(std::floor(lastTick / 65536.0));
    bytes[1424] = static_cast<char>(remainder & 0xFF);
    bytes[1425] = static_cast<char>((remainder >> 8) & 0xFF);
    bytes[1426] = static_cast<char>(large);

    const auto unknown1 = static_cast<uint32_t>(10000000);
    const auto unknown2 = static_cast<uint32_t>(seq.getLastTick() * 5208.333333333333);
    bytes[1428] = static_cast<char>(unknown1 & 0xFF);
    bytes[1429] = static_cast<char>((unknown1 >> 8) & 0xFF);
    bytes[1430] = static_cast<char>((unknown1 >> 16) & 0xFF);
    bytes[1431] = static_cast<char>((unknown1 >> 24) & 0xFF);
    bytes[1432] = static_cast<char>(unknown2 & 0xFF);
    bytes[1433] = static_cast<char>((unknown2 >> 8) & 0xFF);
    bytes[1434] = static_cast<char>((unknown2 >> 16) & 0xFF);
    bytes[1435] = static_cast<char>((unknown2 >> 24) & 0xFF);
    return bytes;
}

std::vector<char> makeBarChunk(mpc::sequencer::Sequence& seq)
{
    std::vector<char> bytes(kSequenceBarChunkLength, char{0});
    int ticksPerBeat = 0;
    int lastTick = 0;

    for (int i = 0; i < seq.getLastBarIndex() + 1; ++i)
    {
        const auto barLength = seq.getBarLength(i);
        lastTick += barLength;
        ticksPerBeat = barLength / seq.getNumerator(i);
        bytes[i * 4] = static_cast<char>(ticksPerBeat);
        bytes[i * 4 + 1] = static_cast<char>(lastTick & 0xFF);
        bytes[i * 4 + 2] = static_cast<char>((lastTick >> 8) & 0xFF);
        bytes[i * 4 + 3] = static_cast<char>((lastTick >> 16) & 0xFF);
    }

    const auto terminatorOffset = static_cast<size_t>((seq.getLastBarIndex() + 1) * 4);
    bytes[terminatorOffset] = static_cast<char>(ticksPerBeat);
    return bytes;
}


std::unique_ptr<all_t::event_t> makeTerminatorEvent()
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(0xFFFFF);
    event->set_terminator_pad(0xF);
    event->set_terminator(std::string(5, static_cast<char>(0xFF)));
    return event;
}

std::unique_ptr<all_t::event_t> makeNoteEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    const auto duration = static_cast<uint32_t>(state.duration);
    event->set_tick(state.tick);
    event->set_duration_bits_1((duration >> 10) & 0x0F);
    event->set_track(trackIndex);
    event->set_duration_bits_2((duration >> 8) & 0x03);
    event->set_id(state.noteNumber);

    auto note = std::make_unique<all_t::note_event_t>(nullptr, event.get(), nullptr);
    note->set_duration_bits_3(duration & 0xFF);
    note->set_velocity(state.velocity);
    note->set_variation_type_bit_1(
        state.noteVariationType == mpc::NoteVariationTypeAttack || state.noteVariationType == mpc::NoteVariationTypeFilter
    );
    note->set_variation_value(state.noteVariationValue);
    note->set_variation_type_bit_2(
        state.noteVariationType == mpc::NoteVariationTypeDecay || state.noteVariationType == mpc::NoteVariationTypeFilter
    );
    event->set_note_event(std::move(note));
    return event;
}

std::unique_ptr<all_t::event_t> makePitchBendEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(state.tick);
    event->set_duration_bits_1(0);
    event->set_track(trackIndex);
    event->set_duration_bits_2(0);
    event->set_id(0xE0);

    auto pitchBend = std::make_unique<all_t::pitch_bend_event_t>(nullptr, event.get(), nullptr);
    uint16_t candidate = static_cast<uint16_t>(state.amount + 16384);
    if (state.amount < 0)
    {
        candidate = static_cast<uint16_t>(state.amount + 8192);
    }
    pitchBend->set_amount_bits_1(candidate & 0xFF);
    pitchBend->set_amount_bits_2((candidate >> 8) & 0xFF);
    pitchBend->set__unnamed2(std::string(1, char{0}));
    event->set_pitch_bend(std::move(pitchBend));
    return event;
}

std::unique_ptr<all_t::event_t> makeControlChangeEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(state.tick);
    event->set_duration_bits_1(0);
    event->set_track(trackIndex);
    event->set_duration_bits_2(0);
    event->set_id(0xB0);

    auto controlChange = std::make_unique<all_t::control_change_event_t>(nullptr, event.get(), nullptr);
    controlChange->set_controller(static_cast<all_t::controller_t>(state.controllerNumber));
    controlChange->set_value(state.controllerValue);
    controlChange->set__unnamed2(std::string(1, char{0}));
    event->set_control_change(std::move(controlChange));
    return event;
}

std::unique_ptr<all_t::event_t> makeProgramChangeEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(state.tick);
    event->set_duration_bits_1(0);
    event->set_track(trackIndex);
    event->set_duration_bits_2(0);
    event->set_id(0xC0);

    auto programChange = std::make_unique<all_t::program_change_event_t>(nullptr, event.get(), nullptr);
    programChange->set_program(state.programChangeProgramIndex);
    programChange->set__unnamed1(std::string(2, char{0}));
    event->set_program_change(std::move(programChange));
    return event;
}

std::unique_ptr<all_t::event_t> makeChannelPressureEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(state.tick);
    event->set_duration_bits_1(0);
    event->set_track(trackIndex);
    event->set_duration_bits_2(0);
    event->set_id(0xD0);

    auto chPressure = std::make_unique<all_t::ch_pressure_event_t>(nullptr, event.get(), nullptr);
    chPressure->set_pressure(state.amount);
    chPressure->set__unnamed1(std::string(2, char{0}));
    event->set_ch_pressure(std::move(chPressure));
    return event;
}

std::unique_ptr<all_t::event_t> makePolyPressureEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(state.tick);
    event->set_duration_bits_1(0);
    event->set_track(trackIndex);
    event->set_duration_bits_2(0);
    event->set_id(0xA0);

    auto polyPressure = std::make_unique<all_t::poly_pressure_event_t>(nullptr, event.get(), nullptr);
    polyPressure->set_note(state.noteNumber);
    polyPressure->set_pressure(state.amount);
    polyPressure->set__unnamed2(std::string(1, char{0}));
    event->set_poly_pressure(std::move(polyPressure));
    return event;
}

std::unique_ptr<all_t::event_t> makeExclusiveEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    auto event = std::make_unique<all_t::event_t>(nullptr, nullptr, nullptr);
    event->set_tick(state.tick);
    event->set_duration_bits_1(0);
    event->set_track(trackIndex);
    event->set_duration_bits_2(0);
    event->set_id(0xF0);

    auto exclusive = std::make_unique<all_t::exclusive_event_t>(nullptr, event.get(), nullptr);
    if (state.type == mpc::sequencer::EventType::Mixer)
    {
        exclusive->set__unnamed0(std::string({9, 0, 0}));
        exclusive->set_bytes(std::string({static_cast<char>(0xF0), 0x47}));

        auto mixer = std::make_unique<all_t::mixer_event_t>(nullptr, exclusive.get(), nullptr);
        mixer->set__unnamed0(std::string({0, 68, 69}));
        auto param = state.mixerParameter;
        if (param == 3)
        {
            param = 4;
        }
        mixer->set_param(static_cast<all_t::mixer_event_param_t>(param + 1));
        mixer->set_pad_index(state.mixerPad);
        mixer->set_value(state.mixerValue);
        mixer->set__unnamed4(std::string({static_cast<char>(0xF7), 0}));
        exclusive->set_mixer(std::move(mixer));

        auto tail = std::string(14, char{0});
        tail[10] = static_cast<char>(0xF8);
        exclusive->set__unnamed3(std::move(tail));
    }
    else
    {
        exclusive->set__unnamed0(std::string({2, 0, 0}));
        exclusive->set_bytes(std::string({static_cast<char>(state.sysExByteA), static_cast<char>(state.sysExByteB)}));

        auto tail = std::string(14, char{0});
        tail[10] = static_cast<char>(0xF8);
        exclusive->set__unnamed3(std::move(tail));
    }

    event->set_exclusive(std::move(exclusive));
    return event;
}

std::unique_ptr<all_t::event_t> makeSequenceEvent(const mpc::sequencer::EventData& state, const uint8_t trackIndex)
{
    switch (state.type)
    {
        case mpc::sequencer::EventType::NoteOn:
            return makeNoteEvent(state, trackIndex);
        case mpc::sequencer::EventType::PitchBend:
            return makePitchBendEvent(state, trackIndex);
        case mpc::sequencer::EventType::ControlChange:
            return makeControlChangeEvent(state, trackIndex);
        case mpc::sequencer::EventType::ProgramChange:
            return makeProgramChangeEvent(state, trackIndex);
        case mpc::sequencer::EventType::ChannelPressure:
            return makeChannelPressureEvent(state, trackIndex);
        case mpc::sequencer::EventType::PolyPressure:
            return makePolyPressureEvent(state, trackIndex);
        case mpc::sequencer::EventType::Mixer:
        case mpc::sequencer::EventType::SystemExclusive:
            return makeExclusiveEvent(state, trackIndex);
        default:
            throw std::runtime_error("Unsupported ALL event type in Kaitai writer");
    }
}

std::vector<char> makeSequenceEventChunk(mpc::sequencer::Sequence& seq)
{
    std::vector<char> chunk;
    for (int tick = 0; tick < seq.getLastTick(); ++tick)
    {
        for (const auto& track : seq.getTracks())
        {
            if (track->getIndex() > 63)
            {
                break;
            }

            for (const auto& event : track->getEvents())
            {
                if (event->getTick() != tick)
                {
                    continue;
                }

                auto kaitaiEvent = makeSequenceEvent(*event->handle, static_cast<uint8_t>(track->getIndex()));
                auto eventBytes = writeStruct(*kaitaiEvent);
                chunk.insert(chunk.end(), eventBytes.begin(), eventBytes.end());
            }
        }
    }

    auto terminator = makeTerminatorEvent();
    auto terminatorBytes = writeStruct(*terminator);
    chunk.insert(chunk.end(), terminatorBytes.begin(), terminatorBytes.end());
    return chunk;
}

std::vector<char> makeSequenceBytes(mpc::sequencer::Sequence& seq, int number)
{
    const auto segmentCountLastEventIndex = countSequenceMetaSegments(seq);
    auto segmentCount = countSequenceBodySegments(seq);
    const auto terminatorCount = (segmentCount & 1) == 0 ? 2 : 1;
    auto bytes = std::vector<char>(
        kSequenceHeaderLength + segmentCount * kSequenceEventSegmentLength + terminatorCount * kSequenceEventSegmentLength,
        char{0}
    );

    const auto sequenceName = padRight(seq.getName(), kVisibleNameLength);
    std::copy(sequenceName.begin(), sequenceName.end(), bytes.begin());

    auto lastEventSegmentIndex = segmentCountLastEventIndex;
    if ((lastEventSegmentIndex & 1) != 0)
    {
        lastEventSegmentIndex--;
    }
    lastEventSegmentIndex /= 2;
    const auto lastEventIndex = static_cast<uint16_t>(1 + (lastEventSegmentIndex < 0 ? 0 : lastEventSegmentIndex));
    bytes[16] = static_cast<char>(lastEventIndex & 0xFF);
    bytes[17] = static_cast<char>((lastEventIndex >> 8) & 0xFF);

    std::copy(std::begin(kSequencePadding1), std::end(kSequencePadding1), bytes.begin() + 19);

    const auto tempo = static_cast<uint16_t>(seq.getInitialTempo() * 10.0);
    bytes[22] = static_cast<char>(tempo & 0xFF);
    bytes[23] = static_cast<char>((tempo >> 8) & 0xFF);
    std::copy(std::begin(kSequencePadding2), std::end(kSequencePadding2), bytes.begin() + 24);

    const auto barCount = static_cast<uint16_t>(seq.getLastBarIndex() + 1);
    bytes[26] = static_cast<char>(barCount & 0xFF);
    bytes[27] = static_cast<char>((barCount >> 8) & 0xFF);

    const auto lastTick = static_cast<uint32_t>(seq.getLastTick());
    const auto lastTickRemainder = static_cast<uint16_t>(lastTick % 65536);
    const auto lastTickHigh = static_cast<uint8_t>(lastTick / 65536);
    bytes[28] = static_cast<char>(lastTickRemainder & 0xFF);
    bytes[29] = static_cast<char>((lastTickRemainder >> 8) & 0xFF);
    bytes[30] = static_cast<char>(lastTickHigh);
    bytes[64] = static_cast<char>(lastTickRemainder & 0xFF);
    bytes[65] = static_cast<char>((lastTickRemainder >> 8) & 0xFF);
    bytes[66] = static_cast<char>(lastTickHigh);

    bytes[18] = static_cast<char>(number);

    const auto unknown1 = static_cast<uint32_t>(10000000);
    const auto unknown2 = static_cast<uint32_t>(seq.getLastTick() * 5208.333333333333);
    for (int i = 0; i < 2; ++i)
    {
        bytes[32 + i * 4] = static_cast<char>(unknown1 & 0xFF);
        bytes[33 + i * 4] = static_cast<char>((unknown1 >> 8) & 0xFF);
        bytes[34 + i * 4] = static_cast<char>((unknown1 >> 16) & 0xFF);
        bytes[35 + i * 4] = static_cast<char>((unknown1 >> 24) & 0xFF);
    }
    for (int i = 2; i < 4; ++i)
    {
        bytes[32 + i * 4] = static_cast<char>(unknown2 & 0xFF);
        bytes[33 + i * 4] = static_cast<char>((unknown2 >> 8) & 0xFF);
        bytes[34 + i * 4] = static_cast<char>((unknown2 >> 16) & 0xFF);
        bytes[35 + i * 4] = static_cast<char>((unknown2 >> 24) & 0xFF);
    }

    const auto loopFirst = static_cast<uint16_t>(seq.getFirstLoopBarIndex());
    auto loopLast = static_cast<uint16_t>(seq.getLastLoopBarIndex());
    bytes[48] = static_cast<char>(loopFirst & 0xFF);
    bytes[49] = static_cast<char>((loopFirst >> 8) & 0xFF);
    if (seq.getLastLoopBarIndex() == mpc::EndOfSequence)
    {
        loopLast = 0xFFFF;
    }
    bytes[50] = static_cast<char>(loopLast & 0xFF);
    bytes[51] = static_cast<char>((loopLast >> 8) & 0xFF);
    bytes[52] = static_cast<char>(seq.isLoopEnabled() ? 1 : 0);

    const auto& startTime = seq.getStartTime();
    bytes[53] = static_cast<char>(startTime.hours);
    bytes[54] = static_cast<char>(startTime.minutes);
    bytes[55] = static_cast<char>(startTime.seconds);
    bytes[56] = static_cast<char>(startTime.frames);
    bytes[57] = static_cast<char>(startTime.frameDecimals);

    std::copy(std::begin(kSequencePadding4), std::end(kSequencePadding4), bytes.begin() + 59);

    for (int i = 0; i < 33; ++i)
    {
        const auto deviceName = i == 0
            ? std::string(8, ' ')
            : padRight(seq.getDeviceName(i - 1), 8);
        std::copy(deviceName.begin(), deviceName.end(), bytes.begin() + 120 + i * 8);
    }

    const auto trackBytes = makeTrackChunk(seq);
    std::copy(trackBytes.begin(), trackBytes.end(), bytes.begin() + 384);

    const auto barBytes = makeBarChunk(seq);
    std::copy(barBytes.begin(), barBytes.end(), bytes.begin() + static_cast<std::ptrdiff_t>(kSequenceBarChunkOffset));

    const auto eventChunk = makeSequenceEventChunk(seq);
    std::copy(eventChunk.begin(), eventChunk.end(), bytes.begin() + static_cast<std::ptrdiff_t>(kSequenceEventsOffset));

    for (size_t i = bytes.size() - 8; i < bytes.size(); ++i)
    {
        bytes[i] = static_cast<char>(0xFF);
    }

    return bytes;
}

std::unique_ptr<std::vector<std::unique_ptr<all_t::sequence_t>>> makeSequences(mpc::Mpc& mpc, all_t& root)
{
    auto result = std::make_unique<std::vector<std::unique_ptr<all_t::sequence_t>>>();

    auto sequencer = mpc.getSequencer();
    const auto usedSequences = sequencer->getUsedSequences();

    if (usedSequences.empty())
    {
        return result;
    }

    result->reserve(usedSequences.size() * 2);

    for (size_t usedSequenceCursor = 0; usedSequenceCursor < usedSequences.size(); ++usedSequenceCursor)
    {
        const auto sequenceIndex = sequencer->getUsedSequenceIndexes()[usedSequenceCursor];
        auto sequenceBytes =
            makeSequenceBytes(*usedSequences[usedSequenceCursor], sequenceIndex + 1);
        result->push_back(parseSection<all_t::sequence_t>(sequenceBytes, root));

        // On disk, an even body segment count is followed by an extra 0xFF
        // terminator segment that Kaitai parses as an empty sequence record.
        if ((countSequenceBodySegments(*usedSequences[usedSequenceCursor]) & 1) == 0)
        {
            result->push_back(
                parseSection<all_t::sequence_t>(std::vector<char>(8, static_cast<char>(0xFF)), root)
            );
        }
    }

    return result;
}


std::vector<char> writeAll(all_t& root)
{
    return writeStruct(root);
}

} // namespace

void AllIo::loadEverything(mpc::Mpc &mpc, mpc::disk::MpcFile *file)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_all_t>(file->getBytes());
    mpc::disk::AllLoader::loadEverythingFromCanonicalBytes(mpc, canonicalBytes);
}

sequence_meta_infos_or_error AllIo::loadSequenceMetaInfos(mpc::Mpc &mpc, mpc::disk::MpcFile *file)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_all_t>(file->getBytes());
    std::istringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::binary
    );
    ::kaitai::kstream parseIo(&parseStream);
    mpc2000xl_all_t parsed(&parseIo);
    parsed._read();

    std::vector<mpc::sequencer::SequenceMetaInfo> result;
    const auto* sequenceMetas = parsed.sequences_metas();
    if (sequenceMetas == nullptr)
    {
        return result;
    }

    for (const auto& sequenceMeta : *sequenceMetas)
    {
        result.push_back(
            {
                sequenceMeta->is_used(),
                sequenceMeta->name()
            }
        );
    }

    return result;
}

std::shared_ptr<mpc::sequencer::Sequence> AllIo::loadOneSequence(
    mpc::Mpc &mpc,
    mpc::disk::MpcFile *file,
    mpc::SequenceIndex sourceIndexInAllFile,
    mpc::SequenceIndex destIndexInMpcMemory)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_all_t>(file->getBytes());
    return mpc::disk::AllLoader::loadOneSequenceFromCanonicalBytes(
        mpc,
        canonicalBytes,
        sourceIndexInAllFile,
        destIndexInMpcMemory
    );
}

std::vector<char> AllIo::save(mpc::Mpc &mpc)
{
    all_t root(nullptr);
    root.set_magic("MPC2KXL ALL 1.00");

    root.set_defaults(makeDefaults(mpc, root));

    root.set_sequencer(makeSequencer(mpc, root));
    root.set__unnamed3(std::string(std::begin(kAllUnknown10), std::end(kAllUnknown10)));
    root.set_count(makeCount(mpc, root));
    root.set_midi_output(makeMidiOutput(mpc, root));
    root.set_midi_input(makeMidiInput(mpc, root));
    root.set_midi_sync(makeMidiSync(mpc, root));
    root.set_song_global(makeSongGlobal(mpc, root));
    root.set__unnamed9(std::string(5, char{0}));
    root.set_locations(makeLocations(mpc, root));
    root.set_misc(makeMisc(mpc, root));
    root.set__unnamed12(std::string(3, char{0}));
    root.set_step_edit_options(makeStepEditOptions(mpc, root));
    root.set_prog_change_to_seq(mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>()->getProgChangeSeq());
    root.set__unnamed15(std::string(78, char{0}));
    root.set_sequences_metas(makeSequenceMetas(mpc, root));
    root.set_songs(makeSongs(mpc, root));
    root.set_sequences(makeSequences(mpc, root));

    return writeAll(root);
}
