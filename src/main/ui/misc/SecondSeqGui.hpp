#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace misc {

			class SecondSeqGui
				: public moduru::observer::Observable
			{

			private:
				int sq{ 0 };

			public:
				void setSq(int i);
				int getSq();

				SecondSeqGui();
			};

		}
	}
}
