#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class AssignScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		AssignScreen(mpc::Mpc& mpc, const int layerIndex);

	public:
		void turnWheel(int i) override;
		void pad(int i, int velo) override;
		void open() override;
		void close() override;
		void update(Observable* o, Message message) override;

	private:
		void displayAssignNote();
		void displayParameter();
		void displayHighRange();
		void displayLowRange();
		void displayAssignNv();

	private:
		std::vector<std::string> typeNames = { "TUNING", "DECAY", "ATTACK", "FILTER" };


	};
}
