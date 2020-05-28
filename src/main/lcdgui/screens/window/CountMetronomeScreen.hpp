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

		int countIn = 0;
		bool inPlay = true;
		int rate = 0;
		bool waitForKey = false;
		bool inRec = true;

	public:
		int getCountInMode();
		bool getInPlay();
		int getRate();
		bool isWaitForKeyEnabled();
		bool getInRec();
		void setCountIn(int i);
		void setInPlay(bool b);
		void setRate(int i);
		void setWaitForKey(bool b);
		void setInRec(bool b);

	private:
		void displayWaitForKey();
		void displayInRec();
		void displayRate();
		void displayInPlay();
		void displayCountIn();

	public:
		CountMetronomeScreen(const int& layer);

	public:
		void open() override;

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	};
}
