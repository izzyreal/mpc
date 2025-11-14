#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/EmptyEvent.hpp>

#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace mpc::sequencer
{
    class NoteOnEvent;
}

namespace mpc::lcdgui::screens
{
    class StepEditorScreen final : public ScreenComponent
    {
        void downOrUp(int increment);
        void adhocPlayNoteEvent(
            const std::shared_ptr<sequencer::NoteOnEvent> &noteEvent) const;
        void adhocPlayNoteEventsAtCurrentPosition() const;

        std::vector<std::shared_ptr<sequencer::Event>>
        computeEventsAtCurrentTick() const;

    public:
        StepEditorScreen(Mpc &mpc, int layerIndex);

        std::vector<std::shared_ptr<sequencer::Event>> computeVisibleEvents(
            const std::vector<std::shared_ptr<sequencer::Event>>
                &eventsAtCurrentTick = {}) const;

        bool visibleEventsEqual(
            const std::vector<std::shared_ptr<sequencer::Event>> &a,
            const std::vector<std::shared_ptr<sequencer::Event>> &b) const;

        void shift();

        void prevStepEvent();
        void nextStepEvent();
        void prevBarStart();
        void nextBarEnd();

        void function(int i) override;
        void turnWheel(int increment) override;
        void left() override;
        void right() override;
        void up() override;
        void down() override;

        void open() override;
        void close() override;
        void openWindow() override;

        void setFromNote(DrumNoteNumber);

    private:
        bool isFirstTickPosChangeAfterScreenHasBeenOpened = true;

        void refreshSelection();
        void refreshEventRows();
        void updateComponents() const;
        void setViewNotesText() const;

        void resetYPosAndYOffset();
        std::string getActiveColumn() const;
        int getActiveRow() const;
        void storeColumnForEventAtActiveRow();
        void restoreColumnForEventAtActiveRow();
        void setSequencerTickPos(const std::function<void()> &tickPosSetter);
        bool paramIsLetter(const std::string &letter) const;

        const std::vector<std::string> viewNames{
            "ALL EVENTS",  "NOTES",       "PITCH BEND", "CTRL:",
            "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE"};
        const std::shared_ptr<sequencer::EmptyEvent> emptyEvent =
            std::make_shared<sequencer::EmptyEvent>();
        std::vector<std::shared_ptr<sequencer::Event>> placeHolder;
        std::shared_ptr<sequencer::Event> selectedEvent;
        std::vector<std::shared_ptr<sequencer::Event>> selectedEvents;
        std::map<std::string, std::string> lastColumn;
        int lastRow = 0;

        int view = 0;
        NoteNumber noteA{MinNoteNumber};
        NoteNumber noteB{MaxNoteNumber};
        int control = -1;
        int yOffset = 0;
        DrumNoteNumber fromNote{NoDrumNoteAssigned};
        int selectionStartIndex = -1;
        int selectionEndIndex = -1;
        std::string selectedParameterLetter;
        bool durationTcPercentageEnabled = false;

        void setView(int i);
        void setNoteA(NoteNumber);
        void setNoteB(NoteNumber);
        void setControl(int i);
        void setyOffset(int i);
        void setSelectedEventIndex(int i);
        void setSelectionEndIndex(int i);
        void setSelectionStartIndex(int i);
        void setSelectedEvents();
        void setSelectedEvent(const std::weak_ptr<sequencer::Event> &event);
        void setSelectedParameterLetter(const std::string &str);
        void checkSelection();
        void removeEvents();
        void displayView() const;

    public:
        std::vector<std::shared_ptr<sequencer::Event>> &getVisibleEvents();
        std::vector<std::shared_ptr<sequencer::Event>> &getSelectedEvents();
        std::shared_ptr<sequencer::Event> getSelectedEvent();
        std::string getSelectedParameterLetter();
        void clearSelection();
        int getYOffset() const;
        std::vector<std::shared_ptr<sequencer::Event>> &getPlaceHolder();
    };
} // namespace mpc::lcdgui::screens
