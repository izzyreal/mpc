#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace midisync {

			class MidiSyncGui
				: public moduru::observer::Observable
			{

			public:
				typedef moduru::observer::Observable super;

			private:
				std::vector<int> modeIns;
				std::vector<int> modeOuts;
				int shiftEarly{};
				bool sendMMCEnabled{};
				int frameRate{};
				//int modeIn{};
				//int modeOut{};
				bool receiveMMCEnabled{};
				int out{};
				int in{};

			public:
				std::vector<int>* getModeIns();
				void setIn(int i);
				int getIn();
				void setOut(int i);
				int getOut();
				int getShiftEarly();
				void setShiftEarly(int i);
				bool isSendMMCEnabled();
				void setSendMMCEnabled(bool b);
				int getFrameRate();
				void setFrameRate(int i);
				int getModeIn();
				void setModeIn(int i);
				int getModeOut();
				void setModeOut(int i);
				bool isReceiveMMCEnabled();
				void setReceiveMMCEnabled(bool b);

				MidiSyncGui();
	
			};

		}
	}
}
