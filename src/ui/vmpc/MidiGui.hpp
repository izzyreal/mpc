#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace vmpc {

			class MidiGui
				: public moduru::observer::Observable
			{

			private:
				int in{ 0 };
				int in1TransmitterNumber{ 0 };
				int in2TransmitterNumber{ -1 };
				int out{ 0 };
				int outAReceiverNumber{ -1 };
				int outBReceiverNumber{ -1 };

			public:
				void setIn(int i);
				int getIn();
				void setOut(int i);
				int getOut();
				void setIn1TransmitterNumber(int i, int max);
				int getIn1TransmitterNumber();
				void setIn2TransmitterNumber(int i, int max);
				int getIn2TransmitterNumber();
				void setOutAReceiverNumber(int i, int max);
				int getOutAReceiverIndex();
				void setOutBReceiverNumber(int i, int max);
				int getOutBReceiverIndex();

				MidiGui();

			};

		}
	}
}

