#pragma once
#include "AbstractSequencerControls.hpp"

namespace mpc::controls::sequencer {

	class SequencerControls
		: public AbstractSequencerControls
	{

	public:
		void init() override;

	public:
		void pressEnter() override;
		void function(int i) override;
		void turnWheel(int i) override;
		void openWindow() override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;

	};
}
