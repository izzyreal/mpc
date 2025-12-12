#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "sequencer/BusType.hpp"

#include <sequencer/TimeSignature.hpp>

namespace mpc::disk
{
    class AllLoader;
}

namespace mpc::engine
{
    class SequencerPlaybackEngine;
}

namespace mpc::sequencer
{
    class Sequence;
    class Sequencer;
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
        void turnWheel(int increment) override;

        int getLastBar() const;
        double getTempo() const;

    private:
        void displayTempo() const;
        void displayLoop() const;
        void displayTsig() const;
        void displayBars() const;
        void displayPgm() const;
        void displayRecordingMode() const;
        void displayBus() const;
        void displayDeviceNumber() const;
        void displayVelo() const;
        void displayDeviceName() const;

        double tempo = 0.0;
        bool loop = false;
        sequencer::BusType busType = sequencer::BusType::MIDI;
        int device = 0;
        int pgm = 0;
        int velo = 0;
        bool recordingModeMulti = false;
        int lastBar = 0;
        sequencer::TimeSignature timeSig;

        void setTempo(double newTempo);
        void setLoop(bool b);
        void setBus(sequencer::BusType);
        void setDeviceNumber(int i);
        void setRecordingModeMulti(bool b);
        void setLastBar(int i);
        void setPgm(int i);
        void setVelo(int i);
        void setTimeSig(int num, int den);

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
        friend class engine::SequencerPlaybackEngine;
    };
} // namespace mpc::lcdgui::screens
