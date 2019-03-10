#pragma once
#include <observer/Observable.hpp>


namespace mpc {
	namespace ui {
		namespace sequencer {
			namespace window {

				class EraseGui
					: public moduru::observer::Observable
				{

				public:
					int track{};
					int erase{};
					int type{};
					int notes0{};
					int notes1{};

				public:
					void setTrack(int i);
					void setErase(int i);
					void setType(int i);
					int getTrack();
					int getErase();
					int getType();

					EraseGui();
				};

			}
		}
	}
}
