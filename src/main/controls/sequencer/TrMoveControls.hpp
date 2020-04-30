#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

namespace mpc {

	namespace ui {
		namespace sequencer {
			class TrMoveGui;
		}
	}

	namespace controls {
		namespace sequencer {

			class TrMoveControls
				: public AbstractSequencerControls
			{

			public:
				typedef AbstractSequencerControls super;

			private:
				std::weak_ptr<mpc::sequencer::Sequence> seq{};
				mpc::ui::sequencer::TrMoveGui* tmGui{ nullptr };

			public:
				void init() override;

			public:
				void turnWheel(int i) override;
				void up() override;
				void down() override;
				void left() override;
				void right() override;
				void function(int i) override;

				TrMoveControls();

			};

		}
	}
}
