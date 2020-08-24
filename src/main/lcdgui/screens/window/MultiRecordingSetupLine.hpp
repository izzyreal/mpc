#pragma once
#include <observer/Observable.hpp>

namespace mpc::lcdgui::screens::window
{
	class MultiRecordingSetupLine
	{

	private:
		int in = 0;
		int track = 0;

	public:
		void setIn(int in);
		int getIn();
		void setTrack(int i);
		int getTrack();

	};
}
