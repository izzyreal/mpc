#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {
	class Mpc;
	namespace sequencer {
		class Sequencer;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}
	
	namespace ui {
		namespace sequencer {

			class NextSeqObserver
				: public moduru::observer::Observer
			{

			private:
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Field> now0Field{};
				std::weak_ptr<mpc::lcdgui::Field> now1Field{};
				std::weak_ptr<mpc::lcdgui::Field> now2Field{};
				std::weak_ptr<mpc::lcdgui::Field> tempoField{};
				std::weak_ptr<mpc::lcdgui::Label> tempoLabel{};
				std::weak_ptr<mpc::lcdgui::Field> tempoSourceField{};
				std::weak_ptr<mpc::lcdgui::Field> timingField{};
				std::weak_ptr<mpc::lcdgui::Field> nextSqField{};
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				mpc::Mpc* mpc;

			private:
				void displaySq();
				void displayNextSq();
				void displayNow0();
				void displayNow1();
				void displayNow2();
				void displayTempo();
				void displayTempoLabel();
				void displayTempoSource();
				void displayTiming();

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

				NextSeqObserver(mpc::Mpc* mpc);
				~NextSeqObserver();

			};

		}
	}
}
