#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/Component.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>

#include <memory>
#include <vector>

namespace mpc::sequencer
{
	class Sequence;
	class Track;
}

namespace mpc::lcdgui::screens::window
{

	class SequenceScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		SequenceScreen(const int& layer);
	
	public:
		void open() override;

	};
}
