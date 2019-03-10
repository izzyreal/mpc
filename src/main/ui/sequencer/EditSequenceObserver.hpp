#pragma once

#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

#include <sequencer/TimeSignature.hpp>

namespace mpc {
	class Mpc;
	namespace sampler {
		class Sampler;
	}

	namespace sequencer {
		class Sequencer;
		class Sequence;
		class Track;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {

		namespace sequencer {

			class EditSequenceGui;

			class EditSequenceObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};

				std::vector<std::string> functionNames{};
				std::vector<std::string> modeNames{};
				EditSequenceGui* editSequenceGui{};
				int trackNum{};
				int seqNum{};
				std::weak_ptr<mpc::sequencer::Sequence> sequence{};
				std::weak_ptr<mpc::sequencer::Track> track{};
				mpc::sequencer::TimeSignature timeSig;
				std::weak_ptr<mpc::lcdgui::Field> editFunctionField{};
				std::weak_ptr<mpc::lcdgui::Field> time0Field{};
				std::weak_ptr<mpc::lcdgui::Field> time1Field{};
				std::weak_ptr<mpc::lcdgui::Field> time2Field{};
				std::weak_ptr<mpc::lcdgui::Field> time3Field{};
				std::weak_ptr<mpc::lcdgui::Field> time4Field{};
				std::weak_ptr<mpc::lcdgui::Field> time5Field{};
				std::weak_ptr<mpc::lcdgui::Field> drumNoteField{};
				std::weak_ptr<mpc::lcdgui::Field> fromSqField{};
				std::weak_ptr<mpc::lcdgui::Field> tr0Field{};
				std::weak_ptr<mpc::lcdgui::Field> toSqField{};
				std::weak_ptr<mpc::lcdgui::Field> tr1Field{};
				std::weak_ptr<mpc::lcdgui::Field> modeField{};
				std::weak_ptr<mpc::lcdgui::Field> start0Field{};
				std::weak_ptr<mpc::lcdgui::Field> start1Field{};
				std::weak_ptr<mpc::lcdgui::Field> start2Field{};
				std::weak_ptr<mpc::lcdgui::Field> copiesField{};
				std::weak_ptr<mpc::lcdgui::Field> midiNote0Field{};
				std::weak_ptr<mpc::lcdgui::Field> midiNote1Field{};
				std::weak_ptr<mpc::lcdgui::Label> midiNote1Label{};
				std::weak_ptr<mpc::lcdgui::Label> toSqLabel{};
				std::weak_ptr<mpc::lcdgui::Label> tr0Label{};
				std::weak_ptr<mpc::lcdgui::Label> tr1Label{};
				std::weak_ptr<mpc::lcdgui::Label> start0Label{};
				std::weak_ptr<mpc::lcdgui::Label> start1Label{};
				std::weak_ptr<mpc::lcdgui::Label> start2Label{};
				std::weak_ptr<mpc::lcdgui::Label> copiesLabel{};
				std::weak_ptr<mpc::lcdgui::Label> fromSqLabel{};
				std::weak_ptr<mpc::lcdgui::Label> modeLabel{};

			private:
				void displayStart();
				void displayTime();
				void setCopiesValue();
				void setModeValue();
				void setEditFunctionValue();

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

			private:
				void setNoteValues();
				void setMidiNoteValues();
				void setDrumNoteValue();

			public:
				EditSequenceObserver(mpc::Mpc* mpc);
				~EditSequenceObserver();
			};

		}
	}
}
