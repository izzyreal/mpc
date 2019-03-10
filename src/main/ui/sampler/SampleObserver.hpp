#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	class Mpc;
	namespace sampler {
		class Sampler;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {

		namespace sampler {

			class SamplerGui;

			class SampleObserver
				: public moduru::observer::Observer
			{

			private:
				const std::string vu_normal = u8"\u00F5";
				const std::string vu_threshold = u8"\u00F6";
				const std::string vu_peak = u8"\u00F8";
				const std::string vu_peak_threshold = u8"\u00F9";
				const std::string vu_normal_threshold = u8"\u00FA";
				const std::string vu_peak_threshold_normal = u8"\u00FB";

				std::vector<std::string> inputNames{ "ANALOG", "DIGITAL" };
				std::vector<std::string> modeNames{ "MONO L", "MONO R", "STEREO" };
				std::vector<std::string> monitorNames{ "OFF", "L/R", "1/2", "3/4", "5/6", "7/8" };

				std::weak_ptr<mpc::lcdgui::Field> inputField{};
				std::weak_ptr<mpc::lcdgui::Field> thresholdField{};
				std::weak_ptr<mpc::lcdgui::Field> modeField{};
				std::weak_ptr<mpc::lcdgui::Field> timeField{};
				std::weak_ptr<mpc::lcdgui::Field> monitorField{};
				std::weak_ptr<mpc::lcdgui::Field> preRecField{};
				std::weak_ptr<mpc::lcdgui::Label> vuLeftLabel{};
				std::weak_ptr<mpc::lcdgui::Label> vuRightLabel{};

				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				SamplerGui* samplerGui{ nullptr };

				bool vu_ready{ false };
				int levelL{ 0 };
				int levelR{ 0 };

			private:
				void displayInput();
				void displayThreshold();
				void displayMode();
				void displayTime();
				void displayMonitor();
				void displayPreRec();
				void updateVU();

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

			public:
				SampleObserver(mpc::Mpc* mpc);
				~SampleObserver();

			};

		}
	}
}
