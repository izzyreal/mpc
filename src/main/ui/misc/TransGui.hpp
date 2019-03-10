#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace misc {

			class TransGui
				: public moduru::observer::Observable
			{

			private:
				int tr{ -1 };
				int amount{ 0 };
				int bar0{ 0 };
				int bar1{ 0 };

			public:
				void setAmount(int i);
				int getAmount();
				void setTr(int i);
				int getTr();
				void setBar0(int i);
				int getBar0();
				void setBar1(int i);
				int getBar1();

				TransGui();
			};

		}
	}
}
