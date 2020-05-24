#pragma once
#include "Component.hpp"

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui
{
	class Label;
	class VerticalBar;
	class MixerTopBackground;
	class MixerFaderBackground;
	class Knob;
}

namespace mpc::lcdgui
{
	class MixerStrip
	{

	private:
		std::vector<std::string> abcd;
		std::vector<std::string> letters;
		std::vector<std::weak_ptr<Component>> mixerStrip;
		int columnIndex;
		std::weak_ptr<Label> tf0;
		std::weak_ptr<Label> tf1;
		std::weak_ptr<Label> tf2;
		std::weak_ptr<Label> tf3;
		std::weak_ptr<Label> tf4;
		std::vector<int> xPos0indiv;
		std::vector<int> xPos1indiv;
		int yPos0indiv;
		int yPos1indiv;
		std::vector<int> xPos0fx;
		std::vector<int> xPos1fx;
		int yPos0fx;
		int yPos1fx;
		std::weak_ptr<VerticalBar> verticalBar;
		std::weak_ptr<Knob> knob;
		std::weak_ptr<MixerTopBackground> mixerTopBackground;
		std::weak_ptr<MixerFaderBackground> mixerFaderBackground;
		int selection;
		std::vector<std::weak_ptr<Label>> labels;

	public:
		std::vector<std::weak_ptr<Component>> getMixerStrip();
		void setValueA(int i);
		void setValueB(int i);
		void initLabels();
		void setColors();
		void setSelection(int i);
		void setValueAString(std::string str);

	public:
		MixerStrip(int columnIndex, int bank);

	};
}
