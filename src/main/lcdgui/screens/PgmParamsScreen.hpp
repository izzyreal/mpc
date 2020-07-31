#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseSamplerControls.hpp>

namespace mpc::lcdgui::screens {

	class PgmParamsScreen
		: public mpc::lcdgui::ScreenComponent
		//public mpc::controls::BaseSamplerControls
	{

	public:

		void function(int i) override;
		void turnWheel(int i) override;
		void openWindow() override;

		PgmParamsScreen(mpc::Mpc& mpc, const int layerIndex);
		
		void open() override;
		void close() override;
		void update(moduru::observer::Observable* observable, nonstd::any message);

	private:
		void displayReson();
		void displayFreq();
		void displayAttackDecay();
		void displayNote();
		void displayPgm();
		void displayTune();
		void displayDecayMode();
		void displayVoiceOverlap();

		std::vector<std::string> decayModes = { "END", "START" };
		std::vector<std::string> voiceOverlapModes = { "POLY", "MONO", "NOTE OFF" };

	};
}
