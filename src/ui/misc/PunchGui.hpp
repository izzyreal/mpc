#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace misc {

			class PunchGui
				: public moduru::observer::Observable
			{

			private:
				int autoPunch{ 0 };
				int time0{ 0 };
				int time1{ 0 };

			public:
				void setAutoPunch(int i);
				int getAutoPunch();
				void setTime0(int l);
				int getTime0();
				void setTime1(int l);
				int getTime1();

				PunchGui();
			};

		}
	}
}
