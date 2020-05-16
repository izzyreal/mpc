#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui::screens::window
{

	class CountMetronomeScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::vector<std::string> countInNames = { "OFF", "REC ONLY", "REC+PLAY" };
		std::vector<std::string> rateNames = { "1/4", "1/4(3)", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };

		std::weak_ptr<mpc::lcdgui::Field> countInField;
		std::weak_ptr<mpc::lcdgui::Field> inPlayField;
		std::weak_ptr<mpc::lcdgui::Field> rateField;
		std::weak_ptr<mpc::lcdgui::Field> inRecField;
		std::weak_ptr<mpc::lcdgui::Field> waitForKeyField;

	private:
		void displayWaitForKey();
		void displayInRec();
		void displayRate();
		void displayInPlay();
		void displayCountIn();

	public:
		CountMetronomeScreen(const int& layer);
	
	public:
		void update(moduru::observer::Observable* observable, nonstd::any message) override;

	public:
		void open() override;
		void close() override;

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	};
}
