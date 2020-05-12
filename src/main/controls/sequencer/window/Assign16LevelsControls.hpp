#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

#include <memory>

namespace mpc::controls::sequencer::window {

	class Assign16LevelsControls
		: public AbstractSequencerControls
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	};
}
