#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class SecondSeqScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		SecondSeqScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		int sq{ 0 };
		void setSq(int i);
		void displaySq();

	};
}
