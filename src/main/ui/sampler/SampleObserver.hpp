#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	class Mpc;
}

namespace mpc::lcdgui {
	class Field;
	class Label;
}

namespace mpc::ui::sampler {
	class SamplerGui;
}

using namespace std;

namespace mpc::ui::sampler {
	class SampleObserver
		: public moduru::observer::Observer
	{

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

		weak_ptr<mpc::lcdgui::Field> inputField{};
		weak_ptr<mpc::lcdgui::Field> thresholdField{};
		weak_ptr<mpc::lcdgui::Field> modeField{};
		weak_ptr<mpc::lcdgui::Field> timeField{};
		weak_ptr<mpc::lcdgui::Field> monitorField{};
		weak_ptr<mpc::lcdgui::Field> preRecField{};
		weak_ptr<mpc::lcdgui::Label> vuLeftLabel{};
		weak_ptr<mpc::lcdgui::Label> vuRightLabel{};

		SamplerGui* samplerGui{ nullptr };

	private:
		void displayInput();
		void displayThreshold();
		void displayMode();
		void displayTime();
		void displayMonitor();
		void displayPreRec();
		void updateVU(float value);

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

	public:
		SampleObserver(mpc::Mpc* mpc);
		~SampleObserver();

	};
}
