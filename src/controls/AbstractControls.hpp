#pragma once

#include <lcdgui/LayeredScreen.hpp>

#include <string>
#include <memory>
#include <vector>

namespace ctoot {
	namespace mpc {
		class MpcSoundPlayerChannel;
	}
}

namespace mpc {

	class Mpc;

	namespace sequencer {
		class Sequencer;
		class Track;
		class NoteEvent;
	}

	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace lcdgui {

		class Field;
		class LayeredScreen;
	}

	namespace ui {

		class NameGui;

		namespace sequencer {
			class SequencerGui;
		}

		namespace sampler {
			class SamplerGui;
		}
	}

	namespace controls {

		class AbstractControls
		{
		protected:
			std::string param{ "" };
			std::string csn{ "" };

			Mpc* mpc{ nullptr };
			std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
			std::weak_ptr<mpc::sampler::Sampler> sampler{};
			std::weak_ptr<mpc::sequencer::Track> track{};
			std::weak_ptr<mpc::sampler::Program> program{};
			ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };

			std::weak_ptr<mpc::lcdgui::LayeredScreen> ls{};
			mpc::ui::NameGui* nameGui{ nullptr };
			ui::sequencer::SequencerGui* sequencerGui{ nullptr };
			ui::sampler::SamplerGui* samplerGui{ nullptr };
			std::weak_ptr<mpc::lcdgui::Field> activeField{};

			int bank_{ 0 };
			std::vector<std::string> typableParams{};

		protected:
			virtual void init();

		public:
			virtual void left();
			virtual void right();
			virtual void up() ;
			virtual void down();
			virtual void function(int i);
			virtual void openWindow();
			virtual void turnWheel(int i);
			//virtual void keyEvent(unsigned char c) {};
			virtual void numpad(int i);
			virtual void pressEnter();
			virtual void rec();
			virtual void overDub();
			virtual void stop();
			virtual void play();
			virtual void playStart();
			virtual void mainScreen();
			virtual void tap();
			virtual void prevStepEvent();
			virtual void nextStepEvent();
			virtual void goTo();
			virtual void prevBarStart();
			virtual void nextBarEnd();
			virtual void nextSeq();
			virtual void trackMute();
			virtual void bank(int i);
			virtual void fullLevel();
			virtual void sixteenLevels();
			virtual void after();
			virtual void shift();
			virtual void undoSeq();
			virtual void erase();

			virtual void setSlider(int i) {};
			virtual bool isTypable();

			virtual void pad(int i, int velo, bool repeat, int tick);
			void generateNoteOn(int nn, int padVelo, int tick);

			void setSliderNoteVar(mpc::sequencer::NoteEvent* n, std::weak_ptr<mpc::sampler::Program> program);

		public:
			AbstractControls(Mpc* mpc);
			virtual ~AbstractControls();

		};

	}
}
