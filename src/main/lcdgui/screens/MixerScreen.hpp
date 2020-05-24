#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/MixerStrip.hpp>

#include <memory>

namespace mpc::lcdgui::screens {

	class MixerScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;
		void pad(int i, int velo, bool repeat, int tick) override;
		void up() override;
		void down() override;
		void left() override;
		void right() override;
		void openWindow() override;

		MixerScreen(const int layerIndex);

		void open() override;
		void close() override;

	private:
		void recordMixerEvent(int pad, int param, int value);

	private:
		const std::vector<std::string> fxPathNames = { "--", "M1", "M2", "R1", "R2" };
		const std::vector<std::string> stereoNames = { "-", "12", "12", "34", "34", "56", "56", "78", "78" };
		const std::vector<std::string> monoNames = { "-", "1", "2", "3", "4", "5", "6", "7", "8" };

	private:
		int tab = 0;
		bool link = false;
		int xPos = 0;
		int yPos = 0;
		std::vector<std::shared_ptr<mpc::lcdgui::MixerStrip>> mixerStrips;
		void initPadNameLabels();
		void initMixerStrips();
		void displayMixerStrips();
		void displayFunctionKeys();
		void displayStereoFaders();
		void displayIndivFaders();
		void displayPanning();
		void displayIndividualOutputs();
		void displayFxPaths();
		void displayFxSendLevels();

		void setLink(bool b);
		void setTab(int i);
		void setXPos(int i);
		void setYPos(int i);

	public:
		int getTab();
		int getXPos();

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	};
}
