#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class TrMuteScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void right() override;
		void turnWheel(int i) override;
		void function(int i) override;

	public:
		void pad(int i, int velo, bool repeat, int tick) override;

	public:
		TrMuteScreen(const int layerIndex);
		void update(moduru::observer::Observable* o, nonstd::any arg) override;
		void open() override;
		void close() override;

	private:
		int bankoffset();
		void displayBank();
		void displayTrackNumbers();
		void displaySq();
		void setOpaque(int i);
		void displayTrack(int i);
		void setTrackColor(int i);
		void displayNow0();
		void displayNow1();
		void displayNow2();
		void refreshTracks();

	};
}
