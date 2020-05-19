#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class TrMoveScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::weak_ptr<mpc::sequencer::Sequence> seq{};
		mpc::ui::sequencer::TrMoveGui* tmGui{ nullptr };

	public:
		void init() override;

	public:
		void turnWheel(int i) override;
		void up() override;
		void down() override;
		void left() override;
		void right() override;
		void function(int i) override;

		TrMoveScreen(const int& layer);

		void open() override;

	private:
		void displayTrLabels();
		void displayTrFields();
		void displaySq();

	};
}