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
		class TwoDots;
		class Wave;
	}

	namespace ui {

		namespace sampler {

			class SoundGui;
			class SamplerGui;

			class TrimObserver
				: public moduru::observer::Observer
			{

			private:
				std::vector<std::string> playXNames{};
				mpc::Mpc* mpc;
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				std::weak_ptr<mpc::sampler::Sound> sound{};
				std::weak_ptr<mpc::lcdgui::Field> sndField{};
				std::weak_ptr<mpc::lcdgui::Field> playXField{};
				std::weak_ptr<mpc::lcdgui::Field> stField{};
				std::weak_ptr<mpc::lcdgui::Field> endField{};
				std::weak_ptr<mpc::lcdgui::Field> viewField{};
				std::weak_ptr<mpc::lcdgui::Field> dummyField{};
				std::weak_ptr<mpc::lcdgui::TwoDots> twoDots;
				std::weak_ptr<mpc::lcdgui::Wave> wave;
				SoundGui* soundGui{ nullptr };
				SamplerGui* samplerGui{ nullptr };

			private:
				void displaySnd();
				void displayPlayX();
				void displaySt();
				void displayEnd();
				void displayView();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			private:
				void waveformLoadData();

			public:
				TrimObserver(mpc::Mpc* mpc);
				~TrimObserver();

			};

		}
	}
}
