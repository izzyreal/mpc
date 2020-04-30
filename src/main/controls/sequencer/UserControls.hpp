#pragma once
#include "AbstractSequencerControls.hpp"

namespace mpc {

	namespace ui {
		class UserDefaults;
	}

	namespace controls {
		namespace sequencer {

			class UserControls
				: public AbstractSequencerControls
			{

			private:
				std::weak_ptr<mpc::ui::UserDefaults> ud{};

			public:
				typedef mpc::controls::sequencer::AbstractSequencerControls super;
				void function(int i) override;
				void turnWheel(int i) override;

				UserControls();
			};

		}
	}
}
