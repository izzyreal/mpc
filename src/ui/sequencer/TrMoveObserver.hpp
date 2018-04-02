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

			class TrMoveGui;

			class TrMoveObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				TrMoveGui* tmGui{};
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Field> tr0Field{};
				std::weak_ptr<mpc::lcdgui::Field> tr1Field{};
				std::weak_ptr<mpc::lcdgui::Label> tr0Label{};
				std::weak_ptr<mpc::lcdgui::Label> tr1Label{};
				std::weak_ptr<mpc::lcdgui::Label> selectTrackLabel{};
				std::weak_ptr<mpc::lcdgui::Label> toMoveLabel{};

			private:
				void displayTrLabels();
				void displayTrFields();

			public:
				void update(moduru::observer::Observable* o, boost::any arg) override;

			private:
				void displaySq();

			public:
				TrMoveObserver(mpc::Mpc* mpc);
				~TrMoveObserver();

			};

		}
	}
}
