#pragma once
#include <observer/Observable.hpp>


namespace mpc {
	namespace ui {
		namespace sequencer {

			class BarCopyGui
				: public moduru::observer::Observable
			{

			public:
				typedef moduru::observer::Observable super;

			private:
				int fromSq{ 0 };
				int toSq{ 0 };
				int lastBar{ 0 };
				int firstBar{ 0 };
				int afterBar{ 0 };
				int copies{ 1 };

			public:
				int getLastBar();
				void setLastBar(int i, int max);
				int getFromSq();
				void setFromSq(int i);
				int getToSq();
				void setToSq(int i);
				int getFirstBar();
				void setFirstBar(int i, int max);
				int getAfterBar();
				void setAfterBar(int i, int max);
				int getCopies();
				void setCopies(int i);

				BarCopyGui();
			};

		}
	}
}
