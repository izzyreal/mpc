#pragma once
#include "AbstractSequencerControls.hpp"

namespace mpc::ui {
	class UserDefaults;
}

namespace mpc::controls::sequencer {

	class UserControls
		: public AbstractSequencerControls
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		UserControls();
	};
}
