#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>

namespace mpc::lcdgui::screens
{
	class SongScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		int step = 0;
		std::weak_ptr<mpc::sequencer::Song> s;

	public:
		void init() override;

	public:
		void up() override;
		void down() override;
		void left() override;
		void right() override;
		void openWindow() override;
		void turnWheel(int i) override;
		void function(int i) override;

		SongScreen(const int& layer);

		void open() override;

	private:
		void displayTempo();
		void displayLoop();
		void displaySteps();
		void displayTempoSource();
		void displayNow0();
		void displayNow1();
		void displayNow2();
		void displaySongName();

	};
}
