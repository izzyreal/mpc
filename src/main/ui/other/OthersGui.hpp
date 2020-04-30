#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc {
	
	namespace ui {
		namespace other {

			class OthersGui
				: public moduru::observer::Observable
			{

			private:
				
				int tapAveraging{ 2 };
				int contrast{ 10 };

			public:
				void setTapAveraging(int i);
				int getTapAveraging();
				void setContrast(int i);
				int getContrast();

			public:
				OthersGui();
				~OthersGui();

			};

		}
	}
}
