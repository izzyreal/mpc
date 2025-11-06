#pragma once

#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class ConvertSongToSeqScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        ConvertSongToSeqScreen(mpc::Mpc &, int layerIndex);

        void open() override;
        void function(int) override;
        void turnWheel(int) override;

    private:
        std::vector<std::string> trackStatusNames{
            "REFERENCED TO 1ST SQ", "OFF TRACKS IGNORED", "MERGED ON MIDI CH."};

        uint8_t toSequenceIndex = 0;
        uint8_t trackStatus = 0;

        void setFromSong(int8_t);

        void setToSequenceIndex(int8_t);

        void setTrackStatus(int8_t);

        void displayFromSong();

        void displayToSequence();

        void displayTrackStatus();

        void convertSongToSeq();
    };
} // namespace mpc::lcdgui::screens::window