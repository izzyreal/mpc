#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class ConvertSongToSeqScreen final : public ScreenComponent
    {
    public:
        ConvertSongToSeqScreen(Mpc &, int layerIndex);

        void open() override;
        void function(int) override;
        void turnWheel(int) override;

    private:
        std::vector<std::string> trackStatusNames{
            "REFERENCED TO 1ST SQ", "OFF TRACKS IGNORED", "MERGED ON MIDI CH."};

        uint8_t toSequenceIndex = 0;
        uint8_t trackStatus = 0;

        void setFromSong(int8_t) const;

        void setToSequenceIndex(int8_t);

        void setTrackStatus(int8_t);

        void displayFromSong() const;

        void displayToSequence() const;

        void displayTrackStatus() const;

        void convertSongToSeq() const;
    };
} // namespace mpc::lcdgui::screens::window