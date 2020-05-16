#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>

namespace mpc::lcdgui::screens::window
{

	class SequenceScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		SequenceScreen(const int& layer);
	
	public:
		void open() override;

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	};
}
