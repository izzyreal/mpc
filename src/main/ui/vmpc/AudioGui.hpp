#pragma once
#include <observer/Observable.hpp>

#include <vector>
#include <memory>

namespace mpc {

	namespace audiomidi {
		class AudioMidiServices;
	}

	namespace ui {
		namespace vmpc {

			class AudioGui
				: public moduru::observer::Observable
			{

			private:
				std::vector<int> inputDevs{};
				std::vector<int> outputDevs{};
				int server{ 0 };
				int in{ 0 };
				int out{ 0 };
				std::weak_ptr<mpc::audiomidi::AudioMidiServices> ams{};
			public:
				void setInputDevs(std::vector<int> devs);
				void setOutputDevs(std::vector<int> devs);
				std::vector<int> getInputDevs();
				std::vector<int> getOutputDevs();
				void setIn(int i);
				int getIn();
				void setOut(int i);
				int getOut();
				void setServer(int i);
				int getServer();
				void setDev0(int i);
				void setDev1(int i);
				int getDev0();
				int getDev1();

				AudioGui(std::weak_ptr<mpc::audiomidi::AudioMidiServices> ams);

			};

		}
	}
}
