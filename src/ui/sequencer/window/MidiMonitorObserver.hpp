#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	class Mpc;

	namespace lcdgui {
		class Label;
	}

	namespace ui {

		namespace sequencer {
			namespace window {

				class MidiMonitorObserver
					: public moduru::observer::Observer
				{

				private:
					mpc::Mpc* mpc;
					std::weak_ptr<mpc::lcdgui::Label> a0{};
					std::weak_ptr<mpc::lcdgui::Label> a1{};
					std::weak_ptr<mpc::lcdgui::Label> a2{};
					std::weak_ptr<mpc::lcdgui::Label> a3{};
					std::weak_ptr<mpc::lcdgui::Label> a4{};
					std::weak_ptr<mpc::lcdgui::Label> a5{};
					std::weak_ptr<mpc::lcdgui::Label> a6{};
					std::weak_ptr<mpc::lcdgui::Label> a7{};
					std::weak_ptr<mpc::lcdgui::Label> a8{};
					std::weak_ptr<mpc::lcdgui::Label> a9{};
					std::weak_ptr<mpc::lcdgui::Label> a10{};
					std::weak_ptr<mpc::lcdgui::Label> a11{};
					std::weak_ptr<mpc::lcdgui::Label> a12{};
					std::weak_ptr<mpc::lcdgui::Label> a13{};
					std::weak_ptr<mpc::lcdgui::Label> a14{};
					std::weak_ptr<mpc::lcdgui::Label> a15{};
					std::weak_ptr<mpc::lcdgui::Label> b0{};
					std::weak_ptr<mpc::lcdgui::Label> b1{};
					std::weak_ptr<mpc::lcdgui::Label> b2{};
					std::weak_ptr<mpc::lcdgui::Label> b3{};
					std::weak_ptr<mpc::lcdgui::Label> b4{};
					std::weak_ptr<mpc::lcdgui::Label> b5{};
					std::weak_ptr<mpc::lcdgui::Label> b6{};
					std::weak_ptr<mpc::lcdgui::Label> b7{};
					std::weak_ptr<mpc::lcdgui::Label> b8{};
					std::weak_ptr<mpc::lcdgui::Label> b9{};
					std::weak_ptr<mpc::lcdgui::Label> b10{};
					std::weak_ptr<mpc::lcdgui::Label> b11{};
					std::weak_ptr<mpc::lcdgui::Label> b12{};
					std::weak_ptr<mpc::lcdgui::Label> b13{};
					std::weak_ptr<mpc::lcdgui::Label> b14{};
					std::weak_ptr<mpc::lcdgui::Label> b15{};

					void runBlinkThread(std::weak_ptr<mpc::lcdgui::Label> label);
					static void static_blink(void * args, std::weak_ptr<mpc::lcdgui::Label> label);

				public:
					void initTimer(std::weak_ptr<mpc::lcdgui::Label> label);
					void update(moduru::observer::Observable* o, boost::any arg) override;

				public:
					MidiMonitorObserver(mpc::Mpc* mpc);
					~MidiMonitorObserver();

				};
	
			}
		}
	}
}
