#pragma once
#include <observer/Observable.hpp>

#include <memory>
#include <vector>

namespace mpc {
	
	namespace sequencer {
		class Event;
	}

	namespace ui {

		namespace sequencer {

			class StepEditorGui
				: public moduru::observer::Observable
			{

			public:
				typedef moduru::observer::Observable super;

			private:
				
				std::vector<std::weak_ptr<mpc::sequencer::Event>> visibleEvents{};
				std::vector <std::weak_ptr<mpc::sequencer::Event>> eventsAtCurrentTick{};
				std::vector<std::weak_ptr<mpc::sequencer::Event>> placeHolder{};
				std::weak_ptr<mpc::sequencer::Event> selectedEvent{};
				std::vector<std::weak_ptr<mpc::sequencer::Event>> selectedEvents{};
				int viewModeNumber{};
				int noteA{};
				int noteB{};
				int controlNumber{};
				bool autoStepIncrementEnabled{};
				bool durationOfRecordedNotes{};
				int tcValueRecordedNotes{};
				int yOffset{};
				int selectedEventNumber{};
				int insertEventType{};
				int editTypeNumber{};
				int changeNoteToNumber{};
				int changeVariationTypeNumber{};
				int changeVariationValue{};
				int editValue{};
				int fromNotePad{};
				int selectionStartIndex{};
				int selectionEndIndex{};
				std::string selectedParameterLetter{};
				bool durationTcPercentageEnabled{};

			public:
				int getInsertEventType();
				void setInsertEventType(int i);
				int getViewModeNumber();
				void setViewModeNumber(int i);
				int getNoteA();
				void setNoteA(int i);
				int getNoteB();
				void setNoteB(int i);
				int getControlNumber();
				void setControlNumber(int i);
				bool isAutoStepIncrementEnabled();
				void setAutoStepIncrementEnabled(bool b);
				bool getDurationOfRecordedNotes();
				void setDurationOfRecordedNotes(bool b);
				int getTcValueRecordedNotes();
				void setTcValueRecordedNotes(int i);
				int getyOffset();
				void setyOffset(int i);
				int getSelectedEventNumber();
				void setSelectedEventNumber(int i);
				void setVisibleEvents(std::vector<std::weak_ptr<mpc::sequencer::Event>> e);
				std::vector<std::weak_ptr<mpc::sequencer::Event>> getVisibleEvents();
				void setEventsAtCurrentTick(std::vector<std::weak_ptr<mpc::sequencer::Event>> l);
				std::vector<std::weak_ptr<mpc::sequencer::Event>> getEventsAtCurrentTick();
				void setFromNotePad(int i);
				int getFromNotePad();
				void setSelectionStartIndex(int i);
				void clearSelection();
				int getSelectionStartIndex();
				void setSelectionEndIndex(int i);
				int getSelectionEndIndex();
				void setSelectedEvents();
				std::vector<std::weak_ptr<mpc::sequencer::Event>> getSelectedEvents();
				void setPlaceHolder(std::vector<std::weak_ptr<mpc::sequencer::Event>> l);
				std::vector<std::weak_ptr<mpc::sequencer::Event>> getPlaceHolder();
				void checkSelection();
				int getEditTypeNumber();
				void setEditTypeNumber(int se_editTypeNumber);
				void setSelectedEvent(std::weak_ptr<mpc::sequencer::Event> event);
				std::weak_ptr<mpc::sequencer::Event> getSelectedEvent();
				void setSelectedParameterLetter(std::string str);
				std::string getParamLetter();
				int getChangeNoteToNumber();
				void setChangeNoteToNumber(int i);
				int getChangeVariationTypeNumber();
				void setChangeVariationTypeNumber(int i);
				int getChangeVariationValue();
				void setChangeVariationValue(int i);
				int getEditValue();
				void setEditValue(int i);
				bool isDurationTcPercentageEnabled();

			public:
				StepEditorGui();
				~StepEditorGui();

			};

		}
	}
}
