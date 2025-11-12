#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::audiomidi
{
    class EventHandler;
    class MidiInput;
} // namespace mpc::audiomidi

namespace mpc::file::all
{
    class MidiSyncMisc;
    class Misc;
} // namespace mpc::file::all

namespace mpc::disk
{
    class AllLoader;
}

namespace mpc::lcdgui::screens
{
    class SyncScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        int getModeOut() const;
        int getModeIn() const;
        int getOut() const;

        SyncScreen(Mpc &, int layerIndex);

        void open() override;

        unsigned char modeIn = 0;
        unsigned char modeOut = 0;

    private:
        const std::vector<std::string> modeNames{"OFF", "MIDI CLOCK",
                                                 "TIME CODE"};

        int tab = 0;

        int shiftEarly = 0;
        bool sendMMCEnabled = false;
        int frameRate = 0;
        bool receiveMMCEnabled = false;
        int out = 0;
        int in = 0;

        void setIn(int i);
        void setOut(int i);
        void setShiftEarly(int i);
        void setSendMMCEnabled(bool b);
        void setFrameRate(int i);
        void setModeIn(unsigned char);
        void setModeOut(unsigned char);

        void setReceiveMMCEnabled(bool b);

        void displayIn() const;
        void displayOut() const;
        void displayModeIn() const;
        void displayModeOut() const;
        void displayReceiveMMC() const;
        void displaySendMMC() const;
        void
        displayShiftEarly() const; // Also used to display "Frame rate:" field

        friend class audiomidi::EventHandler;
        friend class audiomidi::MidiInput;
        friend class disk::AllLoader;
        friend class file::all::MidiSyncMisc;
        friend class file::all::Misc;
    };
} // namespace mpc::lcdgui::screens
