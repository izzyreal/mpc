#pragma once
#include <observer/Observable.hpp>


namespace mpc::ui::sequencer::window {

	class EraseGui
		: public moduru::observer::Observable
	{

	private:
		int track = 0;
		int erase = 0;
		int type = 0;
		int notes0 = 0;
		int notes1 = 0;

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
