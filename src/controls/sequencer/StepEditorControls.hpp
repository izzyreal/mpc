#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {

	namespace sequencer {
		class Event;
	}

	namespace controls {
		namespace sequencer {

			class StepEditorControls
				: public AbstractSequencerControls
			{

			public:
				typedef AbstractSequencerControls super;

			private:
				std::vector<std::weak_ptr<mpc::sequencer::Event>> visibleEvents{};

			private:
				void downOrUp(int increment);

			public:
				void init() override;

			public:
				void function(int i) override;
				void turnWheel(int i) override;
				void prevStepEvent() override;
				void nextStepEvent() override;
				void prevBarStart() override;
				void nextBarEnd() override;
				void left() override;
				void right() override;
				void up() override;
				void down() override;
				void shift() override;

				StepEditorControls(mpc::Mpc* mpc);

			};

		}
	}
}
