#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {
	
	class Mpc;

	namespace sequencer {
		class Song;
		class Sequencer;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {
		namespace sequencer {

			class SongGui;

			class SongObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc{ nullptr };
				std::weak_ptr<mpc::sequencer::Song> song{};
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				std::weak_ptr<mpc::lcdgui::Field> songField{};
				std::weak_ptr<mpc::lcdgui::Field> now0Field{};
				std::weak_ptr<mpc::lcdgui::Field> now1Field{};
				std::weak_ptr<mpc::lcdgui::Field> now2Field{};
				std::weak_ptr<mpc::lcdgui::Field> tempoSourceField{};
				std::weak_ptr<mpc::lcdgui::Field> tempoField{};
				std::weak_ptr<mpc::lcdgui::Field> loopField{};
				std::weak_ptr<mpc::lcdgui::Field> step0Field{};
				std::weak_ptr<mpc::lcdgui::Field> step1Field{};
				std::weak_ptr<mpc::lcdgui::Field> step2Field{};
				std::weak_ptr<mpc::lcdgui::Field> sequence0Field{};
				std::weak_ptr<mpc::lcdgui::Field> sequence1Field{};
				std::weak_ptr<mpc::lcdgui::Field> sequence2Field{};
				std::weak_ptr<mpc::lcdgui::Field> reps0Field{};
				std::weak_ptr<mpc::lcdgui::Field> reps1Field{};
				std::weak_ptr<mpc::lcdgui::Field> reps2Field{};
				mpc::ui::sequencer::SongGui* songGui{};

			private:
				void displayTempo();
				void displayLoop();
				void displaySteps();
				void displayTempoSource();
				void displayNow0();
				void displayNow1();
				void displayNow2();
				void displaySongName();

			public:
				void update(moduru::observer::Observable* o, boost::any arg) override;

				SongObserver(mpc::Mpc* mpc);
				~SongObserver();
			};

		}
	}
}
