#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc {
	class Mpc;
	namespace ui {
		namespace other {

			class OthersGui
				: public moduru::observer::Observable
			{

			private:
				mpc::Mpc* mpc;
				int tapAveraging{ 2 };
				int contrast{ 10 };

			public:
				void setTapAveraging(int i);
				int getTapAveraging();
				void setContrast(int i);
				int getContrast();

			public:
				OthersGui(mpc::Mpc* mpc);
				~OthersGui();

			};

		}
	}
}
