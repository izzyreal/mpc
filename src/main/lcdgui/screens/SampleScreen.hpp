#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {

	class SampleScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;
		void left() override;
		void right() override;
		void up() override;
		void down() override;

	public:
		SampleScreen(const int layerIndex);
		void open() override;
		void close() override;
		void update(moduru::observer::Observable* observable, nonstd::any message);

	private:
		int input{ 0 };
		int threshold{ -20 };
		int mode{ 1 };
		int time{ 100 };
		int monitor{ 0 };
		int preRec{ 100 };

		void setInput(int i);
		void setThreshold(int i);
		void setMode(int i);
		void setTime(int i);
		void setMonitor(int i);
		void setPreRec(int i);

		void displayInput();
		void displayThreshold();
		void displayMode();
		void displayTime();
		void displayMonitor();
		void displayPreRec();
		void updateVU(float value);

	public:
		int getMode();
		int getMonitor();

	private:
		const string vu_normal = u8"\u00F5";
		const string vu_threshold = u8"\u00F6";
		const string vu_peak = u8"\u00F8";
		const string vu_peak_threshold = u8"\u00F9";
		const string vu_normal_threshold = u8"\u00FA";
		const string vu_peak_threshold_normal = u8"\u00FB";

		vector<string> inputNames{ "ANALOG", "DIGITAL" };
		vector<string> modeNames{ "MONO L", "MONO R", "STEREO" };
		vector<string> monitorNames{ "OFF", "L/R", "1/2", "3/4", "5/6", "7/8" };

	};
}
