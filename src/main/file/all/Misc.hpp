#pragma once

#include <memory>
#include <vector>

#include "lcdgui/screens/window/LocateScreen.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::file::all
{
    class Misc
    {
    public:
        static const int LENGTH = 131;

        std::vector<std::pair<int, int>> getSwitches();
        int getTapAvg() const;
        bool isInReceiveMMCEnabled() const;
        bool isAutoStepIncEnabled() const;
        bool isDurationOfRecNotesTc() const;
        int getDurationTcPercentage() const;
        bool isPgmChToSeqEnabled() const;
        std::vector<mpc::lcdgui::screens::window::LocateScreen::Location> &
        getLocations();

        std::vector<char> &getBytes();

        Misc(const std::vector<char> &b);
        Misc(mpc::Mpc &);

    private:
        static const int LOCATIONS_OFFSET = 0;
        static const int TAP_AVG_OFFSET = 36;
        static const int MIDI_SYNC_IN_RECEIVE_MMC_OFFSET = 37;
        static const int MIDI_SWITCH_OFFSET = 38;
        static const int AUTO_STEP_INCREMENT_OFFSET = 49;
        static const int DURATION_OF_REC_NOTES_OFFSET = 50;
        static const int DURATION_TC_PERCENTAGE_OFFSET = 51;
        static const int MIDI_PGM_CHANGE_TO_SEQ_OFFSET = 52;

        int tapAvg;
        bool inReceiveMMCEnabled;
        bool autoStepInc;
        bool durationOfRecNotesTcEnabled;
        int durationTcPercentage;
        bool pgmChToSeqEnabled;
        std::vector<char> saveBytes;
        std::vector<std::pair<int, int>> switches =
            std::vector<std::pair<int, int>>(4);
        std::vector<mpc::lcdgui::screens::window::LocateScreen::Location>
            locations = std::vector<
                mpc::lcdgui::screens::window::LocateScreen::Location>(
                9, {0, 0, 0});
    };
} // namespace mpc::file::all
