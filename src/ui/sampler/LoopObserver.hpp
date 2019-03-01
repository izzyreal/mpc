#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	class Mpc;
	namespace sampler {
		class Sampler;
		class Sound;
	}

	namespace lcdgui {
		class Field;
		class Wave;
		class TwoDots;
	}

	namespace ui {

		namespace sampler {
			
			class SoundGui;

			class LoopObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				SoundGui* soundGui{ nullptr };
				std::vector<std::string> playXNames{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				std::weak_ptr<mpc::sampler::Sound> sound{};
				std::weak_ptr<mpc::lcdgui::Field> sndField{};
				std::weak_ptr<mpc::lcdgui::Field> playXField{};
				std::weak_ptr<mpc::lcdgui::Field> toField{};
				std::weak_ptr<mpc::lcdgui::Field> endLengthField{};
				std::weak_ptr<mpc::lcdgui::Field> endLengthValueField{};
				std::weak_ptr<mpc::lcdgui::Field> loopField{};
				std::weak_ptr<mpc::lcdgui::Field> dummyField{};
				std::weak_ptr<mpc::lcdgui::TwoDots> twoDots;
				std::weak_ptr<mpc::lcdgui::Wave> wave;

			private:
				void displaySnd();
				void displayPlayX();
				void displayTo();
				void displayEndLength();
				void displayEndLengthValue();
				void displayLoop();

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

			private:
				void waveformLoadData();

			public:
				LoopObserver(mpc::Mpc* mpc);
				~LoopObserver();

			};

		}
	}
}
