#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {
	
	namespace sequencer {
		class Sequencer;
		class Sequence;
		class Track;
		class TimeSignature;
	}

	namespace lcdgui {
		class Field;
	}

	namespace ui {

		namespace sequencer {

			class BarCopyGui;

			class BarCopyObserver
				: public moduru::observer::Observer
			{

			private:
				BarCopyGui* bcGui{};
				int trackNum{};
				int seqNum{};
				std::weak_ptr<mpc::lcdgui::Field> fromSqField{};
				std::weak_ptr<mpc::lcdgui::Field> toSqField{};
				std::weak_ptr<mpc::lcdgui::Field> firstBarField{};
				std::weak_ptr<mpc::lcdgui::Field> lastBarField{};
				std::weak_ptr<mpc::lcdgui::Field> afterBarField{};
				std::weak_ptr<mpc::lcdgui::Field> copiesField{};

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			private:

			public:
				BarCopyObserver();
				~BarCopyObserver();
			};

		}
	}
}
