#pragma once

#include <memory>
#include <thread>

namespace mpc {

	namespace lcdgui {
		class Label;
	}

	namespace ui {

		namespace sequencer {
			namespace window {

				class MidiMonitorTimer
				{

				private:
					std::thread blinkThread;

				public:
					MidiMonitorTimer(std::weak_ptr<mpc::lcdgui::Label> label);
					~MidiMonitorTimer();

				};
	
			}
		}
	}
}
