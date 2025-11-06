#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <vector>
#include <functional>

namespace mpc::sequencer
{
    class Sequence;
    class Track;
} // namespace mpc::sequencer

namespace mpc::lcdgui::screens
{
    class SequencerScreen final : public ScreenComponent
    {

    public:
        SequencerScreen(Mpc &mpc, int layerIndex);

        void erase();
        void tap();

    private:
        void setTrackToUsedIfItIsCurrentlyUnused() const;
        void displaySq() const;
        void displayTempoSource() const;
        void displayVelo() const;
        void displayDeviceNumber() const;
        void displayBus() const;
        void displayBars() const;
        void displayPgm() const;
        void displayDeviceName() const;
        void displayTempo() const;
        void displayTempoLabel() const;
        void displayOn() const;
        void displayTr() const;
        void displayCount() const;
        void displayTiming() const;

        // Moving the cursor is wrapped in order to handle some alternative
        // destination mapping while there is a next sequence queued up.
        void moveCursor(const std::function<void()> &cursorCall);

    public:
        // Shared with NextSeqObserver, find better home
        static std::vector<std::string> timingCorrectNames;

    private:
        static std::vector<std::string> busNames;
        void displayNow0() const;
        void displayNow1() const;
        void displayNow2() const;
        void displayRecordingMode() const;
        void displayTsig() const;
        void displayLoop() const;
        void displayPunchWhileRecording();
        void displayNextSq() const;

    public:
        void pressEnter() override;
        void function(int i) override;
        void turnWheel(int i) override;
        void openWindow() override;
        void left() override;
        void right() override;
        void up() override;
        void down() override;
        void play() override;
        void playStart() override;
        void stop() override;
        void rec() override;
        void overDub() override;
        void open() override;
        void close() override;

        void setPunchRectOn(int i, bool b);

        // During certain erase and note repeat scenarios, the function keys are
        // hidden and replaced by a bespoke label.
        // `hideFooterLabelAndShowFunctionKeys` is invoked when the function
        // keys should be shown again, and the the bespoke label hidden.
        void hideFooterLabelAndShowFunctionKeys();
    };
} // namespace mpc::lcdgui::screens
