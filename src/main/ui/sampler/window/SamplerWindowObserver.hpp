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
	}

	namespace lcdgui {
		class Field;
		class Label;
	}


	namespace ui {

		namespace sampler {

			class SamplerGui;

			namespace window {

				class SamplerWindowGui;

				class SamplerWindowObserver
					: public moduru::observer::Observer
				{

				private:
					std::vector<std::string> letters{};
					
					std::weak_ptr<mpc::sampler::Sampler> sampler{};
					ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };
					std::weak_ptr<mpc::sampler::Program> program{};
					std::string csn{ "" };
					mpc::ui::sampler::SamplerGui* samplerGui{ nullptr };
					SamplerWindowGui* swGui{ nullptr };
					std::weak_ptr<mpc::lcdgui::Field> midiProgramChangeField{};
					std::weak_ptr<mpc::lcdgui::Field> programNameField{};
					std::weak_ptr<mpc::lcdgui::Field> pgmField{};
					std::weak_ptr<mpc::lcdgui::Field> newNameField{};
					std::weak_ptr<mpc::lcdgui::Field> pgm0Field{};
					std::weak_ptr<mpc::lcdgui::Field> pgm1Field{};
					std::weak_ptr<mpc::lcdgui::Label> info0Label{};
					std::weak_ptr<mpc::lcdgui::Label> info1Label{};
					std::weak_ptr<mpc::lcdgui::Label> info2Label{};
					std::weak_ptr<mpc::lcdgui::Field> a0Field{};
					std::weak_ptr<mpc::lcdgui::Field> b0Field{};
					std::weak_ptr<mpc::lcdgui::Field> c0Field{};
					std::weak_ptr<mpc::lcdgui::Field> d0Field{};
					std::weak_ptr<mpc::lcdgui::Field> a1Field{};
					std::weak_ptr<mpc::lcdgui::Field> b1Field{};
					std::weak_ptr<mpc::lcdgui::Field> c1Field{};
					std::weak_ptr<mpc::lcdgui::Field> d1Field{};
					std::weak_ptr<mpc::lcdgui::Field> a2Field{};
					std::weak_ptr<mpc::lcdgui::Field> b2Field{};
					std::weak_ptr<mpc::lcdgui::Field> c2Field{};
					std::weak_ptr<mpc::lcdgui::Field> d2Field{};
					std::weak_ptr<mpc::lcdgui::Field> a3Field{};
					std::weak_ptr<mpc::lcdgui::Field> b3Field{};
					std::weak_ptr<mpc::lcdgui::Field> c3Field{};
					std::weak_ptr<mpc::lcdgui::Field> d3Field{};
					std::weak_ptr<mpc::lcdgui::Field> initPadAssignField{};
					std::weak_ptr<mpc::lcdgui::Field> prog0Field{};
					std::weak_ptr<mpc::lcdgui::Field> prog1Field{};
					std::weak_ptr<mpc::lcdgui::Field> note0Field{};
					std::weak_ptr<mpc::lcdgui::Field> note1Field{};
					std::weak_ptr<mpc::lcdgui::Field> noteField{};
					std::weak_ptr<mpc::lcdgui::Field> veloAttackField{};
					std::weak_ptr<mpc::lcdgui::Field> veloStartField{};
					std::weak_ptr<mpc::lcdgui::Field> veloLevelField{};
					std::weak_ptr<mpc::lcdgui::Field> veloField{};
					std::weak_ptr<mpc::lcdgui::Field> attackField{};
					std::weak_ptr<mpc::lcdgui::Field> decayField{};
					std::weak_ptr<mpc::lcdgui::Field> amountField{};
					std::weak_ptr<mpc::lcdgui::Field> veloFreqField{};
					std::weak_ptr<mpc::lcdgui::Field> tuneField{};
					std::weak_ptr<mpc::lcdgui::Field> veloPitchField{};
					std::weak_ptr<mpc::lcdgui::Field> autoChromAssSndField{};
					std::weak_ptr<mpc::lcdgui::Field> sourceField{};
					std::weak_ptr<mpc::lcdgui::Field> originalKeyField{};
					std::weak_ptr<mpc::lcdgui::Field> nameForNewSoundField{};
					std::weak_ptr<mpc::lcdgui::Field> assignToNoteField{};

				private:
					void displayNameForNewSound();
					void displayAssignToPad();
					void displaySource();
					void displayTune();
					void displayVeloPitch();
					void displayAttack();
					void displayDecay();
					void displayAmount();
					void displayVeloFreq();
					void displayNote();
					void displayVeloAttack();
					void displayVeloStart();
					void displayVeloLevel();
					void displayVelo();
					void displayProg0();
					void displayNote0();
					void displayProg1();
					void displayNote1();
					void displayInitPadAssign();

				public:
					void update(moduru::observer::Observable* o, nonstd::any arg) override;

				private:
					void displayOriginalKey();
					void displayAutoChromAssSnd();
					void displayAssignmentView();
					void displayInfo0();
					void displayInfo1();
					void displayInfo2();
					void displayPad(int i);
					void displayPgm0();
					void displayPgm1();
					void displayNewName();
					void displayPgm();
					void displayProgramName();
					void displayMidiProgramChange();

				public:
					SamplerWindowObserver();
					~SamplerWindowObserver();

				};

			}
		}
	}
}
