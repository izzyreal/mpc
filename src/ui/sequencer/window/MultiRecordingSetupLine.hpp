#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace sequencer {
			namespace window {

				class MultiRecordingSetupLine
					: public moduru::observer::Observable
				{

				private:
					int in{};
					int track{};
					int out{};

				public:
					int getIn();
					void setTrack(int i);
					int getTrack();
					void setOut(int i);
					int getOut();

					MultiRecordingSetupLine(int inputNumber);

				};

			}
		}
	}
}
