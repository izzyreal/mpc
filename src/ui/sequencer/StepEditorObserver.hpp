#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <string>
#include <vector>

#include <ui/sequencer/EventRow.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/EmptyEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

namespace mpc {
	class Mpc;
	namespace sequencer {
		class Sequencer;
		class Sequence;
		class Track;
	}

	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}
	
	namespace ui {
		namespace sequencer {

			class StepEditorGui;
			class StepEditorObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				std::vector<std::string> viewNames{};
				std::vector<std::weak_ptr<mpc::sequencer::Event>> visibleEvents;
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				std::weak_ptr<mpc::sequencer::Sequence> sequence{};
				std::weak_ptr<mpc::sequencer::Track> track{};
				std::weak_ptr<mpc::lcdgui::Field> viewField{};
				std::weak_ptr<mpc::lcdgui::Field> controlNumberField{};
				std::weak_ptr<mpc::lcdgui::Field> fromNoteField{};
				std::weak_ptr<mpc::lcdgui::Field> toNoteField{};
				std::weak_ptr<mpc::lcdgui::Field> now0Field{};
				std::weak_ptr<mpc::lcdgui::Field> now1Field{};
				std::weak_ptr<mpc::lcdgui::Field> now2Field{};
				std::weak_ptr<mpc::lcdgui::Label> controlNumberLabel{};
				std::weak_ptr<mpc::lcdgui::Label> fromNoteLabel{};
				std::weak_ptr<mpc::lcdgui::Label> toNoteLabel{};
				StepEditorGui* stepEditorGui{};
				std::vector<std::weak_ptr<mpc::sequencer::Event>> eventsAtCurrentTick{};
				std::vector<std::unique_ptr<EventRow>> eventRows{};
				std::weak_ptr<mpc::sampler::Program> program{};
				std::shared_ptr<mpc::sequencer::EmptyEvent> emptyEvent;
				
			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

			private:
				void refreshSelection();
				void initVisibleEvents();

			public:
				void refreshEventRows();
				void refreshViewModeNotes();
				void setViewModeNotesText();

				StepEditorObserver(mpc::Mpc* mpc);
				~StepEditorObserver();

			};

		}
	}
}
