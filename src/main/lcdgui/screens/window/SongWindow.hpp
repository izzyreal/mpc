#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
	class SongWindow
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		SongWindow(mpc::Mpc& mpc, const int layerIndex);
	
	public:
		void open() override;

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	};
}
