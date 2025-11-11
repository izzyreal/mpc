#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <sequencer/TimeSignature.hpp>

namespace mpc::disk
{
    class AllLoader;
}

namespace mpc::sequencer
{
    class Sequence;
    class Sequencer;
    class SequencerPlaybackEngine;
} // namespace mpc::sequencer

namespace mpc::nvram
{
    class NvRam;
}

namespace mpc::file::all
{
    class Defaults;
}

namespace mpc::lcdgui::screens
{
    class InitScreen;
    class SequencerScreen;
    class EventsScreen;
    class BarsScreen;
} // namespace mpc::lcdgui::screens

namespace mpc::lcdgui::screens
{
    class UserScreen final : public ScreenComponent
    {

    public:
        UserScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void function(int i) override;
        void turnWheel(int i) override;

        int getLastBar() const;
        double getTempo() const;

    private:
        std::vector<std::string> busNames{"MIDI", "DRUM1", "DRUM2", "DRUM3",
                                          "DRUM4"};

        void displayTempo() const;
        void displayLoop() const;
        void displayTsig() const;
        void displayBars() const;
        void displayPgm() const;
        void displayRecordingMode() const;
        void displayBus();
        void displayDeviceNumber() const;
        void displayVelo() const;
        void displayDeviceName();

        double tempo = 0.0;
        bool loop = false;
        int bus = 0;
        int device = 0;
        int pgm = 0;
        int velo = 0;
        bool recordingModeMulti = false;
        std::string sequenceName;
        std::vector<std::string> trackNames;
        int lastBar = 0;
        sequencer::TimeSignature timeSig;
        std::vector<std::string> deviceNames;

        void setTempo(double newTempo);
        void setLoop(bool b);
        void setBus(int i);
        void setDeviceNumber(int i);
        void setRecordingModeMulti(bool b);
        void setLastBar(int i);
        void setPgm(int i);
        void setVelo(int i);
        void setDeviceName(int i, const std::string &s);
        void setSequenceName(const std::string &name);
        void setTimeSig(int num, int den);
        void setTrackName(int i, const std::string &s);

        std::string getDeviceName(int i);
        std::string getTrackName(int i);
        int8_t getTrackStatus() const;

        void resetPreferences();

        friend class InitScreen;
        friend class disk::AllLoader;
        friend class file::all::Defaults;
        friend class SequencerScreen;
        friend class EventsScreen;
        friend class BarsScreen;
        friend class nvram::NvRam;
        friend class sequencer::Sequence;
        friend class sequencer::Sequencer;
        friend class sequencer::SequencerPlaybackEngine;
    };
} // namespace mpc::lcdgui::screens
