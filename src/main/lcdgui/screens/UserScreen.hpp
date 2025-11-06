#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <sequencer/TimeSignature.hpp>

namespace mpc
{
    class Util;
}

namespace mpc::disk
{
    class AllLoader;
}

namespace mpc::sequencer
{
    class Sequence;
    class Sequencer;
    class FrameSeq;
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
    class UserScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

    public:
        UserScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;

    private:
        std::vector<std::string> busNames{"MIDI", "DRUM1", "DRUM2", "DRUM3",
                                          "DRUM4"};

        void displayTempo();
        void displayLoop();
        void displayTsig();
        void displayBars();
        void displayPgm();
        void displayRecordingMode();
        void displayBus();
        void displayDeviceNumber();
        void displayVelo();
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
        mpc::sequencer::TimeSignature timeSig;
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
        int8_t getTrackStatus();

        void resetPreferences();

        friend class InitScreen;
        friend class mpc::disk::AllLoader;
        friend class mpc::file::all::Defaults;
        friend class mpc::lcdgui::screens::SequencerScreen;
        friend class mpc::lcdgui::screens::EventsScreen;
        friend class mpc::lcdgui::screens::BarsScreen;
        friend class mpc::nvram::NvRam;
        friend class mpc::sequencer::Sequence;
        friend class mpc::sequencer::Sequencer;
        friend class mpc::sequencer::FrameSeq;
        friend class mpc::Util;
    };
} // namespace mpc::lcdgui::screens
