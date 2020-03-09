#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	class Mpc;
	namespace lcdgui {
		class Field;
	}

	namespace sequencer {
		class Sequence;
	}

	namespace ui {
		namespace misc {

			class PunchGui;

			class PunchObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				PunchGui* punchGui{ nullptr };
				std::weak_ptr<mpc::lcdgui::Field> autoPunchField{};
				std::weak_ptr<mpc::lcdgui::Field> time0Field{};
				std::weak_ptr<mpc::lcdgui::Field> time1Field{};
				std::weak_ptr<mpc::lcdgui::Field> time2Field{};
				std::weak_ptr<mpc::lcdgui::Field> time3Field{};
				std::weak_ptr<mpc::lcdgui::Field> time4Field{};
				std::weak_ptr<mpc::lcdgui::Field> time5Field{};
				mpc::sequencer::Sequence* sequence{ nullptr };
				std::vector<std::string> autoPunchNames{};

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

			private:
				void displayAutoPunch();
				void displayTime();
				void displayBackground();

			public:
				PunchObserver(mpc::Mpc* mpc);
				~PunchObserver();
			};

		}
	}
}
