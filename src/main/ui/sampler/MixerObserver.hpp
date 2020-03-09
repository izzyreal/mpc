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

	namespace lcdgui {
		class MixerStrip;
		class LayeredScreen;
		class Field;
	}

	namespace sequencer {
		class Sequencer;
		class Sequence;
		class Track;
	}

	namespace sampler {
		class Sampler;
		class Program;
	}
	
	namespace ui {
		namespace sampler {

			class SamplerGui;
			class MixerGui;
			class MixerSetupGui;

			class MixerObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				SamplerGui* samplerGui{ nullptr };
				MixerSetupGui* mixerSetupGui{ nullptr };
				MixerGui* mixGui{ nullptr };
				std::vector<std::string> fxPathNames{};
				std::vector<std::string> stereoNames{};
				std::vector<std::string> monoNames{};
				std::weak_ptr<mpc::lcdgui::LayeredScreen> ls{};
				int bank{};
				ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };
				std::weak_ptr<mpc::sampler::Program> program{};
				std::vector<mpc::lcdgui::MixerStrip*> mixerStrips{};
				std::weak_ptr<mpc::lcdgui::Field> masterLevelField{};
				std::weak_ptr<mpc::lcdgui::Field> fxDrumField{};
				std::weak_ptr<mpc::lcdgui::Field> stereoMixSourceField{};
				std::weak_ptr<mpc::lcdgui::Field> indivFxSourceField{};
				std::weak_ptr<mpc::lcdgui::Field> copyPgmMixToDrumField{};
				std::weak_ptr<mpc::lcdgui::Field> recordMixChangesField{};

			private:
				void initPadNameLabels();
				void initMixerStrips();
				void displayMixerStrips();
				void displayMasterLevel();
				void displayFxDrum();
				void displayStereoMixSource();
				void displayIndivFxSource();
				void displayCopyPgmMixToDrum();
				void displayRecordMixChanges();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;
				void displayFunctionKeys();

			public:
				MixerObserver(mpc::Mpc* mpc);
				~MixerObserver();

			};

		}
	}
}
