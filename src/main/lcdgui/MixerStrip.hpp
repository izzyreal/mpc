#pragma once
#include "Component.hpp"

#include <memory>
#include <vector>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::lcdgui
{
	class Label;
	class MixerFader;
	class MixerTopBackground;
	class MixerFaderBackground;
	class Knob;
}

namespace mpc::lcdgui
{
	class MixerStrip
		: public mpc::lcdgui::Component
	{

	private:
		mpc::Mpc& mpc;
		const std::vector<std::string> abcd{ "A", "B", "C", "D" };
		const std::vector<int> xPos0indiv{ 5, 20, 35, 50, 65, 80, 95, 110, 125, 140, 155, 170, 185, 200, 215, 230 };
		const std::vector<int> xPos1indiv{ 12, 27, 42, 57, 72, 87, 102, 117, 132, 147, 162, 177, 192, 207, 222, 237 };
		const std::vector<int> xPos0fx{ 5, 20, 35, 50, 65, 80, 95, 110, 125, 140, 155, 170, 185, 200, 215, 230 };
		const std::vector<int> xPos1fx{ 11, 26, 41, 56, 71, 86, 101, 116, 131, 146, 161, 176, 191, 206, 221, 236 };
		
		int columnIndex;
		int yPos0indiv;
		int yPos1indiv;
		int yPos0fx;
		int yPos1fx;
		int selection;

	private:
		std::shared_ptr<MixerFader> findMixerFader();
		std::shared_ptr<Knob> findKnob();
		std::shared_ptr<MixerTopBackground> findMixerTopBackground();
		std::shared_ptr<MixerFaderBackground> findMixerFaderBackground();

	public:
		void setValueA(int i);
		void setValueB(int i);
		void initLabels();
		void setColors();
		void setSelection(int i);
		void setValueAString(std::string str);
		void setBank(int i);

	public:
		MixerStrip(mpc::Mpc& mpc, int columnIndex);

	};
}
