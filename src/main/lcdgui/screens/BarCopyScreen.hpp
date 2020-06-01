#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class BarCopyScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int j) override;
		void turnWheel(int i) override;

		BarCopyScreen(const int layerIndex);
		void open() override;

	private:
		void displayCopies();
		void displayToSq();
		void displayFromSq();
		void displayAfterBar();
		void displayLastBar();
		void displayFirstBar();

	private:
		int fromSq = 0;
		int toSq = 0;
		int lastBar = 0;
		int firstBar = 0;
		int afterBar = 0;
		int copies = 1;
		void setLastBar(int i, int max);
		void setToSq(int i);
		void setFirstBar(int i, int max);
		void setAfterBar(int i, int max);
		void setCopies(int i);

	public:
		void setFromSq(int i);
		int getFromSq();

	};
}
