#pragma once

#include <observer/Observable.hpp>

#include <vector>
#include <memory>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
	class TopPanel;
	class Button;
	class DataWheel;
	class Pot;
	class Led;
	class Slider;
	class HwPad;
}

namespace mpc::hardware
{
	class Hardware final
		: public moduru::observer::Observable
	{

	private:
		std::shared_ptr<TopPanel> topPanel;
		std::vector<std::shared_ptr<HwPad>> pads;
		std::vector<std::shared_ptr<Button>> buttons;
		std::vector<std::shared_ptr<Led>> leds;

		std::shared_ptr<DataWheel> dataWheel;
		std::shared_ptr<Slider> slider;
		std::shared_ptr<Pot> recPot;
		std::shared_ptr<Pot> volPot;

	public:
		std::weak_ptr<TopPanel> getTopPanel();
		std::weak_ptr<HwPad> getPad(int index);
		std::weak_ptr<Button> getButton(std::string label);
		std::weak_ptr<Led> getLed(std::string label);
		std::vector<std::shared_ptr<Led>> getLeds();
		std::weak_ptr<DataWheel> getDataWheel();
		std::weak_ptr<Pot> getRecPot();
		std::weak_ptr<Pot> getVolPot();
		std::weak_ptr<Slider> getSlider();

	public:
		Hardware(mpc::Mpc& mpc);

	};
}
