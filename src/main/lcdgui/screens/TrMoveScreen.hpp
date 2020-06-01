#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class TrMoveScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void up() override;
		void down() override;
		void left() override;
		void right() override;
		void function(int i) override;

		TrMoveScreen(const int layerIndex);

		void open() override;
		
	private:
		void displayTrLabels();
		void displayTrFields();
		void displaySq();

	private:
		int sq = 0;
		int selectedTrackIndex = -1;
		int currentTrackIndex = 0;

	public:
		bool isSelected();
		void goUp();
		void goDown();
		void setSq(int i);
		void select();
		void cancel();
		void insert(mpc::sequencer::Sequence* s);

	};
}