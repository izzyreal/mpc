#include "Hardware.hpp"

#include <Mpc.hpp>

#include "Button.hpp"
#include "DataWheel.hpp"
#include "HwPad.hpp"
#include "Led.hpp"
#include "Pot.hpp"
#include "HwSlider.hpp"

using namespace mpc::hardware;
using namespace std;

Hardware::Hardware(mpc::Mpc* mpc)
{
	vector<string> buttonLabels{ "left", "right", "up", "down",	"rec", "overdub", "stop", "play", "playstart", "mainscreen", "prevstepevent", "nextstepevent",	"goto",	"prevbarstart",	"nextbarend", "tap", "nextseq",	"trackmute", "openwindow", "fulllevel", "sixteenlevels", "f1", "f2", "f3", "f4", "f5", "f6", "shift", "enter", "undoseq", "erase", "after", "banka", "bankb", "bankc", "bankd" };
	/*
	static std::vector<int> bankKeys",
	static std::vector<int> padKeys",
	static std::vector<int> altNumPad",
	*/
	for (auto& l : buttonLabels)
		buttons.push_back(std::make_shared<Button>(mpc, l));

	for (int i = 0; i <= 9; i++)
		buttons.push_back(std::make_shared<Button>(mpc, std::to_string(i)));
	
	for (int i = 0; i <= 15; i++)
		pads.push_back(std::make_shared<HwPad>(mpc, i));

	dataWheel = make_shared<DataWheel>(mpc);

	vector<string> ledLabels{ "fulllevel", "sixteenlevels", "nextseq", "trackmute", "padbanka", "padbankb", "padbankc", "padbankd", "after", "undoseq", "rec", "overdub", "play" };

	for (auto& l : ledLabels)
		leds.push_back(std::make_shared<Led>(l));

	recPot = make_shared<Pot>("rec", mpc);
	volPot = make_shared<Pot>("vol", mpc);

	slider = make_shared<Slider>(mpc);
}

weak_ptr<Pot> Hardware::getRecPot() {
	return recPot;
}

weak_ptr<Pot> Hardware::getVolPot() {
	return volPot;
}

weak_ptr<HwPad> Hardware::getPad(int index) {
	return pads.at(index);
}

weak_ptr<Button> Hardware::getButton(std::string label) {
	for (auto b : buttons)
		if (b->getLabel().compare(label) == 0) return b;
	return weak_ptr<Button>();
}


weak_ptr<Led> Hardware::getLed(std::string label) {
	for (auto l : leds)
		if (l->getLabel().compare(label) == 0) return l;
	return weak_ptr<Led>();
}

vector<shared_ptr<Led> > Hardware::getLeds() {
	return leds;
}

weak_ptr<DataWheel> Hardware::getDataWheel() {
	return dataWheel;
}

weak_ptr<Slider> Hardware::getSlider() {
	return slider;
}

Hardware::~Hardware() {
}
