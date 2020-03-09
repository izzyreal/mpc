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
		class Label;
	}

	namespace ui {

		namespace sampler {

			class SoundGui;

			class SndParamsObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				std::vector<std::string> playXNames{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				std::weak_ptr<mpc::sampler::Sound> sound{};
				std::weak_ptr<mpc::lcdgui::Field> sndField{};
				std::weak_ptr<mpc::lcdgui::Field> playXField{};
				std::weak_ptr<mpc::lcdgui::Field> levelField{};
				std::weak_ptr<mpc::lcdgui::Field> tuneField{};
				std::weak_ptr<mpc::lcdgui::Field> beatField{};
				std::weak_ptr<mpc::lcdgui::Label> sampleTempoLabel{};
				std::weak_ptr<mpc::lcdgui::Label> newTempoLabel{};
				std::weak_ptr<mpc::lcdgui::Field> dummyField{};
				SoundGui* soundGui{ nullptr };

			private:
				void displayLevel();
				void displayTune();
				void displayBeat();
				void displaySampleAndNewTempo();
				void displaySnd();
				void displayPlayX();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

				SndParamsObserver(mpc::Mpc* mpc);
				~SndParamsObserver();

			};

		}
	}
}
