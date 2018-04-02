#pragma once
#include <observer/Observable.hpp>

namespace mpc {

	namespace sequencer {
		class Sequence;
	}

	namespace ui {
		namespace sequencer {

			class TrMoveGui
				: public moduru::observer::Observable
			{

			public:
				typedef moduru::observer::Observable super;

			private:
				int sq{};
				int selectedTrackIndex{ -1 };
				int currentTrackIndex{ 0 };

			public:
				bool isSelected();
				int getSq();
				void goUp();
				void goDown();
				int getCurrentTrackIndex();
				void setSq(int i);
				int getSelectedTrackIndex();
				void select();
				void cancel();
				void insert(mpc::sequencer::Sequence* s);

				TrMoveGui();

			};

		}
	}
}
