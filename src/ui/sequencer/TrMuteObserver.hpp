#pragma once
#include <observer/Observer.hpp>

#include <vector>
#include <memory>

namespace mpc {
	class Mpc;
	namespace sequencer {
		class Sequencer;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {
		namespace sampler {
			class SamplerGui;
		}

		namespace sequencer {

			class TrMuteObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				mpc::ui::sampler::SamplerGui* samplerGui{};
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Field> now0Field{};
				std::weak_ptr<mpc::lcdgui::Field> now1Field{};
				std::weak_ptr<mpc::lcdgui::Field> now2Field{};
				std::vector<std::weak_ptr<mpc::lcdgui::Label>> tracks{};
				std::weak_ptr<mpc::lcdgui::Label> tr1{};
				std::weak_ptr<mpc::lcdgui::Label> tr2{};
				std::weak_ptr<mpc::lcdgui::Label> tr3{};
				std::weak_ptr<mpc::lcdgui::Label> tr4{};
				std::weak_ptr<mpc::lcdgui::Label> tr5{};
				std::weak_ptr<mpc::lcdgui::Label> tr6{};
				std::weak_ptr<mpc::lcdgui::Label> tr7{};
				std::weak_ptr<mpc::lcdgui::Label> tr8{};
				std::weak_ptr<mpc::lcdgui::Label> tr9{};
				std::weak_ptr<mpc::lcdgui::Label> tr10{};
				std::weak_ptr<mpc::lcdgui::Label> tr11{};
				std::weak_ptr<mpc::lcdgui::Label> tr12{};
				std::weak_ptr<mpc::lcdgui::Label> tr13{};
				std::weak_ptr<mpc::lcdgui::Label> tr14{};
				std::weak_ptr<mpc::lcdgui::Label> tr15{};
				std::weak_ptr<mpc::lcdgui::Label> tr16{};
				std::weak_ptr<mpc::lcdgui::Label> trackNumbers{};
				std::weak_ptr<mpc::lcdgui::Label> bank{};

			private:
				int bankoffset();
				void displayBank();
				void displayTrackNumbers();
				void displaySq();
				void setOpaque(int i);
				void displayTrack(int i);
				void setTrackColor(int i);

			public:
				void update(moduru::observer::Observable* o, boost::any arg) override;

			private:
				void displayNow0();
				void displayNow1();
				void displayNow2();
				void refreshTracks();

			public:
				TrMuteObserver(mpc::Mpc* mpc);
				~TrMuteObserver();

			};

		}
	}
}
