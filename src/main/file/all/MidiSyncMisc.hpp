#pragma once

#include <string>
#include <memory>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::file::all
{
    class MidiSyncMisc
    {

    public:
        static const int LENGTH = 24;

    private:
        static const int IN_MODE_OFFSET = 0;
        static const int OUT_MODE_OFFSET = 1;
        static const int SHIFT_EARLY_OFFSET = 2;
        static const int SEND_MMC_OFFSET = 3;
        static const int FRAME_RATE_OFFSET = 4;
        static const int INPUT_OFFSET = 5;
        static const int OUTPUT_OFFSET = 6;
        static const int DEF_SONG_NAME_OFFSET = 7;
        static const int
            SONG_MODE_IGNORE_TEMPO_CHANGE_EVENTS_IN_SEQUENCE_OFFSET = 23;

    public:
        int inMode;
        int outMode;
        int shiftEarly;
        bool sendMMCEnabled;
        int frameRate;
        int input;
        int output;
        std::string defSongName{""};
        bool songModeIgnoreTempoChangeEventsInSequence;

        std::vector<char> saveBytes;

    public:
        int getInMode();
        int getOutMode();
        int getShiftEarly();
        bool isSendMMCEnabled();
        int getFrameRate();
        int getInput();
        int getOutput();
        std::string getDefSongName();
        bool getSongModeIgnoreTempoChangeEvents();

    public:
        std::vector<char> &getBytes();

        MidiSyncMisc(const std::vector<char> &b);
        MidiSyncMisc(mpc::Mpc &mpc);
    };
} // namespace mpc::file::all
