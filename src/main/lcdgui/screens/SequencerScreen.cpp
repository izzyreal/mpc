#include "SequencerScreen.hpp"

#include <lcdgui/Parameter.hpp>

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SequencerScreen::SequencerScreen(std::map<std::string, std::unique_ptr<Component>> componentMap)
	: Component("sequencer-screen")
{
	this->componentMap = move(componentMap);	
}

void SequencerScreen::Draw(std::vector<std::vector<bool>>* pixels)
{
	auto param = dynamic_cast<Parameter*>(componentMap["sq"].get());
	param->getLabel().lock()->setText("foo");
	param->Draw(pixels);
}