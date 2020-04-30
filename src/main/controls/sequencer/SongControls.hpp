#pragma once
#include <controls/sequencer/AbstractSequencerControls.hpp>

#include <memory>

namespace mpc {

	namespace sequencer {
		class Song;
	}

	namespace controls {
		namespace sequencer {

			class SongControls
				: public AbstractSequencerControls
			{

			public:
				typedef AbstractSequencerControls super;

			private:
				int step{};
				std::weak_ptr<mpc::sequencer::Song> s{};

			public:
				void init() override;

			public:
				void up() override;
				void down() override;
				void left() override;
				void right() override;
				void openWindow() override;
				void turnWheel(int i) override;
				void function(int i) override;

				SongControls();

			};

		}
	}
}
