#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/EventRow.hpp>
#include <lcdgui/EventRowParameters.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/EmptyEvent.hpp>

#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace mpc::lcdgui::screens
{
	class StepEditorScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		void downOrUp(int increment);
        void adhocPlayNoteEvent(const std::shared_ptr<mpc::sequencer::NoteOnEvent>& noteEvent);
        void adhocPlayNoteEventsAtCurrentPosition();

	public:
		StepEditorScreen(mpc::Mpc& mpc, const int layerIndex);

		void shift();

		void prevStepEvent();
		void nextStepEvent();
		void prevBarStart();
		void nextBarEnd();

		void function(int i) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;

		void open() override;
		void close() override;
        void openWindow() override;

		void update(Observable*, Message message) override;

        void setFromNote(int i);

    private:
		void refreshSelection();
		void initVisibleEvents();
        void refreshEventRows();
		void updateComponents();
		void setViewNotesText();

        void resetYPosAndYOffset();
        std::string getActiveColumn();
        int getActiveRow();
        void storeColumnForEventAtActiveRow();
        void restoreColumnForEventAtActiveRow();
        void setSequencerTickPos(const std::function<void()>& tickPosSetter);
        bool paramIsLetter(const std::string& letter);

	private:
        int playSingleEventCounter = 0;
		const std::vector<std::string> viewNames { "ALL EVENTS", "NOTES", "PITCH BEND", "CTRL:", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE" };
		const std::shared_ptr<mpc::sequencer::EmptyEvent> emptyEvent = std::make_shared<mpc::sequencer::EmptyEvent>();
		std::vector<std::shared_ptr<mpc::sequencer::Event>> visibleEvents;
		std::vector<std::shared_ptr<mpc::sequencer::Event>> eventsAtCurrentTick;
		std::vector<std::shared_ptr<mpc::sequencer::Event>> placeHolder;
		std::shared_ptr<mpc::sequencer::Event> selectedEvent;
		std::vector<std::shared_ptr<mpc::sequencer::Event>> selectedEvents;
		std::map<std::string, std::string> lastColumn;
		int lastRow = 0;

		int view = 0;
		int noteA = 0;
		int noteB = 127;
		int control = -1;
		int yOffset = 0;
		int fromNote = 34;
		int selectionStartIndex = -1;
		int selectionEndIndex = -1;
		std::string selectedParameterLetter;
		bool durationTcPercentageEnabled = false;

	private:
		void setView(int i);
		void setNoteA(int i);
		void setNoteB(int i);
		void setControl(int i);
		void setyOffset(int i);
		void setSelectedEventIndex(int i);
		void setSelectionEndIndex(int i);
        void setSelectionStartIndex(int i);
		void setSelectedEvents();
		void setSelectedEvent(std::weak_ptr<mpc::sequencer::Event> event);
		void setSelectedParameterLetter(std::string str);
		void checkSelection();
		void removeEvents();
		void displayView();

	public:
		std::vector<std::shared_ptr<mpc::sequencer::Event>>& getVisibleEvents();
		std::vector<std::shared_ptr<mpc::sequencer::Event>>& getSelectedEvents();
		std::shared_ptr<mpc::sequencer::Event> getSelectedEvent();
		std::string getSelectedParameterLetter();
		void clearSelection();
		int getYOffset();
		std::vector<std::shared_ptr<mpc::sequencer::Event>>& getPlaceHolder();

	};
}
