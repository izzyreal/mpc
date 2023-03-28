#pragma once

#include <string>
#include <memory>
#include "sequencer/NoteEvent.hpp"

#include "BaseControls.hpp"

namespace mpc {
	namespace sampler {
		class Sampler;
		class Program;
	}
	namespace sequencer {
		class Sequencer;
		class Track;
	}
	namespace controls {

		class GlobalReleaseControls
			: public BaseControls
		{

		public:
			void simplePad(int padIndexWithBank);

		private:
			void handlePlayNoteOff(std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly> onEvent);

		public:
			void overDub() override;
			void rec() override;
            void play() override;
            void tap() override;
			void shift() override;
			void erase() override;
			void function(int i) override;
			void goTo() override;

		public:
			// Not implemented, in case a release is triggered. We don't want BaseControls to start processing these.
			void left() override {};
			void right() override {};
			void up() override {};
			void down() override {};
			void numpad(int i) override {};
			void pressEnter() override {};
			void stop() override {};
			void playStart() override {};
			void mainScreen() override {};
			void prevStepEvent() override {};
			void nextStepEvent() override {};
			void prevBarStart() override {};
			void nextBarEnd() override {};
			void nextSeq() override {};
			void trackMute() override {};
			void bank(int i) override {};
			void fullLevel() override {};
			void sixteenLevels() override {};
			void after() override {};
			void undoSeq() override {};

		public:
			GlobalReleaseControls(mpc::Mpc& mpc);

		};

	}
}
