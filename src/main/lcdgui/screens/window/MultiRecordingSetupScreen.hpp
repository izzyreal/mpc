#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/window/MultiRecordingSetupLine.hpp>

namespace mpc::lcdgui::screens::window {

	class MultiRecordingSetupScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		int yPos = 0;
		int yOffset = 0;
		std::vector<MultiRecordingSetupLine> mrsLines = std::vector<MultiRecordingSetupLine>(34);
		std::vector<MultiRecordingSetupLine*> visibleMrsLines;
		std::vector<std::string> inNames = std::vector<std::string>(34);

	public:
		void init() override;

	public:
		void left() override;
		void right() override;
		void up() override;
		void down() override;
		void turnWheel(int i) override;
		void function(int i) override;

	public:
		MultiRecordingSetupScreen(mpc::Mpc& mpc, const int layerIndex);

	public:
		void open() override;

	private:
		void displayMrsLine(int i);
		void setYOffset(int i);
		void setMrsTrack(int inputNumber, int newTrackNumber);

	public:
		std::vector<MultiRecordingSetupLine*> getMrsLines();

	};
}
