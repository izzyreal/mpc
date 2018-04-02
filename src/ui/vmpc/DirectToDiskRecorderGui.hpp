#pragma once
#include <observer/Observable.hpp>

#include <memory>
#include <string>

namespace mpc {
	class Mpc;
	namespace ui {

		namespace vmpc {

			class DirectToDiskRecorderGui
				: public moduru::observer::Observable
			{

			private:
				mpc::Mpc* mpc;
				int record{ 0 };
				int sq{ 0 };
				int song{ 0 };
				int time0{ 0 };
				int time1{ 0 };
				std::string outputFolder{ "" };
				bool offline{ false };
				bool splitLR{ true };
				int sampleRate{ 0 };

			public:
				int getTime0();
				void setTime0(int time0);
				int getTime1();
				void setTime1(int time1);
				void setRecord(int i);
				int getRecord();
				void setSq(int i);
				int getSq();
				void setSong(int i);
				int getSong();
				void setOutputFolder(std::string s);
				std::string getOutputfolder();
				void setOffline(bool b);
				bool isOffline();
				void setSplitLR(bool b);
				bool isSplitLR();
				void setSampleRate(int rate);
				int getSampleRate();

				DirectToDiskRecorderGui(mpc::Mpc* mpc);

			};

		}
	}
}
