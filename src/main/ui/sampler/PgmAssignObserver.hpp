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

	

	namespace sampler {
		class Sampler;
		class Program;
		class NoteParameters;
		class Pad;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {
		namespace sampler {

			class SamplerGui;

			class PgmAssignObserver
				: public moduru::observer::Observer
			{

			private:
				const std::vector<std::string> soundGenerationModes{ "NORMAL", "SIMULT", "VEL SW", "DCY SW" };
				mpc::sampler::NoteParameters* lastNp = nullptr;
				mpc::sampler::Pad* lastPad = nullptr;
				std::weak_ptr<mpc::sampler::Sampler> sampler{ };
				SamplerGui* samplerGui{ nullptr };
				ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };
				std::weak_ptr<mpc::sampler::Program> program{};
				std::weak_ptr<mpc::lcdgui::Field> velocityRangeLowerField{};
				std::weak_ptr<mpc::lcdgui::Field> velocityRangeUpperField{};
				std::weak_ptr<mpc::lcdgui::Field> optionalNoteAField{};
				std::weak_ptr<mpc::lcdgui::Field> optionalNoteBField{};
				std::weak_ptr<mpc::lcdgui::Field> pgmField{};
				std::weak_ptr<mpc::lcdgui::Field> selectedPadNumberField{};
				std::weak_ptr<mpc::lcdgui::Field> padNoteField{};
				std::weak_ptr<mpc::lcdgui::Field> padAssignModeField{};
				std::weak_ptr<mpc::lcdgui::Field> selectedNoteField{};
				std::weak_ptr<mpc::lcdgui::Field> sndNumberField{};
				std::weak_ptr<mpc::lcdgui::Field> soundGenerationModeField{};
				std::weak_ptr<mpc::lcdgui::Label> velocityRangeLowerLabel{};
				std::weak_ptr<mpc::lcdgui::Label> velocityRangeUpperLabel{};
				std::weak_ptr<mpc::lcdgui::Label> optionalNoteALabel{};
				std::weak_ptr<mpc::lcdgui::Label> optionalNoteBLabel{};
				std::weak_ptr<mpc::lcdgui::Label> isSoundStereoLabel{};

			private:
				void displayPgm();
				void displaySoundName();
				void displayPadAssignMode();
				void displayPadNote();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			private:
				void displaySoundGenerationMode();
				void displayVeloRangeUpper();
				void displayVeloRangeLower();
				void displayOptionalNoteB();
				void displayOptionalNoteA();
				void displaySelectedNote();
				void displaySelectedPadName();

			public:
				PgmAssignObserver();
				~PgmAssignObserver();

			};

		}
	}
}
