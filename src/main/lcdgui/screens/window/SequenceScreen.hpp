#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class SequenceScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		SequenceScreen(const int layerIndex);
	
	public:
		void open() override;

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	};
}
