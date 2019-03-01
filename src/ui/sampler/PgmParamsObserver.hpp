#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace ctoot {
	namespace mpc {
		class MpcSoundPlayerChannel;
	}
}

namespace mpc {

	class Mpc;

	namespace sampler {
		class Sampler;
		class Program;
		class NoteParameters;
	}

	namespace lcdgui {
		class LayeredScreen;
		class Field;
		class Label;
	}

	namespace ui {
		namespace sampler {

			class SamplerGui;

			class PgmParamsObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::sampler::NoteParameters* lastNp = nullptr;
				mpc::Mpc* mpc;
				std::vector<std::string> decayModes{};
				std::vector<std::string> voiceOverlapModes{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				std::weak_ptr<mpc::sampler::Program> program{};
				ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };
				std::weak_ptr<mpc::lcdgui::Field> pgmField{};
				std::weak_ptr<mpc::lcdgui::Field> noteField{};
				std::weak_ptr<mpc::lcdgui::Field> attackField{};
				std::weak_ptr<mpc::lcdgui::Field> decayField{};
				std::weak_ptr<mpc::lcdgui::Field> decayModeField{};
				std::weak_ptr<mpc::lcdgui::Field> freqField{};
				std::weak_ptr<mpc::lcdgui::Field> resonField{};
				std::weak_ptr<mpc::lcdgui::Field> tuneField{};
				std::weak_ptr<mpc::lcdgui::Field> voiceOverlapField{};
				mpc::lcdgui::LayeredScreen* ls {};
				SamplerGui* samplerGui{ nullptr };

			public:
				void update(moduru::observer::Observable* o, std::any arg) override;

			private:
				void displayReson();
				void displayFreq();
				void displayAttackDecay();
				void displayNote();
				void displayPgm();
				void displayTune();
				void displayDecayMode();
				void displayVoiceOverlap();

			public:
				PgmParamsObserver(mpc::Mpc* mpc);
				~PgmParamsObserver();

			};

		}
	}
}
