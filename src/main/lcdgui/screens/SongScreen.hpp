#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>

namespace mpc::lcdgui::screens::window
{
	class SongWindow;
	class NameScreen;
}

namespace mpc::lcdgui::screens
{
	class SongScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		int offset = -1;
		int activeSongIndex = 0;
		std::string defaultSongName = "Song";
		bool loop = false;

		void setSelectedSongIndex(int i);

	public:
		void setOffset(int i);
		void setDefaultSongName(std::string s);
		void setLoop(bool b);
		std::string getDefaultSongName();
		int getOffset();
		int getActiveSongIndex();
		bool isLoopEnabled();

	public:
		void up() override;
		void down() override;
		void left() override;
		void right() override;
		void openWindow() override;
		void turnWheel(int i) override;
		void function(int i) override;

		SongScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
		void close() override;
		void update(moduru::observer::Observable*, nonstd::any message);

	private:
		void displayTempo();
		void displayLoop();
		void displaySteps();
		void displayTempoSource();
		void displayNow0();
		void displayNow1();
		void displayNow2();
		void displaySongName();

		friend class mpc::lcdgui::screens::window::SongWindow;
		friend class mpc::lcdgui::screens::window::NameScreen;

	};
}
