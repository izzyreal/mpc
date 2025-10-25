#include "file/all/Misc.hpp"

#include <Mpc.hpp>

#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/OthersScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include "file/ByteUtil.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::file::all;

Misc::Misc(const std::vector<char> &b)
{
    for (int i = 0; i < 9; i++)
    {
        const auto locationOffset = LOCATIONS_OFFSET + (i * 4);
        const uint16_t locationBarIndex = ByteUtil::bytes2ushort({b[locationOffset], b[locationOffset + 1]});
        const uint8_t locationBeatIndex = b[locationOffset + 2];
        const uint8_t locationClock = b[locationOffset + 3];
        locations[i] = {locationBarIndex, locationBeatIndex, locationClock};
    }

    tapAvg = b[TAP_AVG_OFFSET];
    inReceiveMMCEnabled = b[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] > 0;

    for (int i = 0; i < 4; i++)
    {
        auto ctrl = static_cast<int>(b[MIDI_SWITCH_OFFSET + (i * 2)]);
        auto func = b[MIDI_SWITCH_OFFSET + (i * 2) + 1];
        switches[i] = std::pair(ctrl == 0xFF ? -1 : ctrl, func);
    }

    autoStepInc = b[AUTO_STEP_INCREMENT_OFFSET] > 0;
    durationOfRecNotesTcEnabled = b[DURATION_OF_REC_NOTES_OFFSET] > 0;
    durationTcPercentage = b[DURATION_TC_PERCENTAGE_OFFSET];
    pgmChToSeqEnabled = b[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] > 0;
}

Misc::Misc(mpc::Mpc &mpc)
{
    saveBytes = std::vector<char>(LENGTH);

    auto &locationsToPersist = mpc.screens->get<LocateScreen>()->getLocations();

    assert(locationsToPersist.size() == 9);

    for (int i = 0; i < 9; i++)
    {
        const auto locationBarBytes = ByteUtil::ushort2bytes(std::get<0>(locationsToPersist[i]));
        const auto locationBeatByte = std::get<1>(locationsToPersist[i]);
        const auto locationClockByte = std::get<2>(locationsToPersist[i]);
        const auto locationOffset = LOCATIONS_OFFSET + (i * 4);
        saveBytes[locationOffset] = locationBarBytes[0];
        saveBytes[locationOffset + 1] = locationBarBytes[1];
        saveBytes[locationOffset + 2] = locationBeatByte;
        saveBytes[locationOffset + 3] = locationClockByte;
    }

    auto stepEditOptionsScreen = mpc.screens->get<StepEditOptionsScreen>();
    auto othersScreen = mpc.screens->get<OthersScreen>();

    saveBytes[TAP_AVG_OFFSET] = (char)(othersScreen->getTapAveraging() - 2);

    auto syncScreen = mpc.screens->get<SyncScreen>();

    saveBytes[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] = (char)(syncScreen->receiveMMCEnabled ? 1 : 0);

    auto midiSwScreen = mpc.screens->get<MidiSwScreen>();

    for (int i = 0; i < 4; i++)
    {
        auto ctrl = midiSwScreen->getSwitch(i).first;
        auto func = midiSwScreen->getSwitch(i).second;
        saveBytes[MIDI_SWITCH_OFFSET + (i * 2)] = ctrl == - 1 ? 0xFF : ctrl;
        saveBytes[MIDI_SWITCH_OFFSET + (i * 2) + 1] = (char)func;
    }

    saveBytes[AUTO_STEP_INCREMENT_OFFSET] = (char)(stepEditOptionsScreen->isAutoStepIncrementEnabled() ? 1 : 0);
    saveBytes[DURATION_OF_REC_NOTES_OFFSET] = (char)(stepEditOptionsScreen->isDurationOfRecordedNotesTcValue() ? 1 : 0);
    saveBytes[DURATION_TC_PERCENTAGE_OFFSET] = (char)(stepEditOptionsScreen->getTcValuePercentage());
    const auto midiInputScreen = mpc.screens->get<MidiInputScreen>();
    saveBytes[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] = midiInputScreen->getProgChangeSeq() ? 0x01 : 0x00;
}

int Misc::getTapAvg()
{
    return tapAvg;
}

bool Misc::isInReceiveMMCEnabled()
{
    return inReceiveMMCEnabled;
}

bool Misc::isAutoStepIncEnabled()
{
    return autoStepInc;
}

bool Misc::isDurationOfRecNotesTc()
{
    return durationOfRecNotesTcEnabled;
}

int Misc::getDurationTcPercentage()
{
    return durationTcPercentage;
}

bool Misc::isPgmChToSeqEnabled()
{
    return pgmChToSeqEnabled;
}

std::vector<std::pair<int, int>> Misc::getSwitches()
{
    return switches;
}

std::vector<LocateScreen::Location> &Misc::getLocations()
{
    return locations;
}

std::vector<char> &Misc::getBytes()
{
    return saveBytes;
}
