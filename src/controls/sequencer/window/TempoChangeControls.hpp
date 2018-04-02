#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

#include <memory>

namespace mpc {

	namespace sequencer {
		class TempoChangeEvent;
	}

	namespace controls {
		namespace sequencer {
			namespace window {

				class TempoChangeControls
					: public AbstractSequencerControls
				{

				public:
					typedef AbstractSequencerControls super;

				private:
					std::weak_ptr<mpc::sequencer::TempoChangeEvent> previous{};
					std::weak_ptr<mpc::sequencer::TempoChangeEvent> current{};
					std::weak_ptr<mpc::sequencer::TempoChangeEvent> next{};
					int yPos{};

				public:
					void left() override;
					void right() override;
					void function(int j) override;
					void init() override;
					void turnWheel(int j) override;
					void down() override;
					void up() override;

					TempoChangeControls(mpc::Mpc* mpc);

				};

			}
		}
	}
}
