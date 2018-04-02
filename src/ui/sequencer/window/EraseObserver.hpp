#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc {
	class Mpc;
	namespace sequencer {
		class Sequence;
	}

	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}
	
	namespace ui {
		namespace sequencer {
			namespace window {

				class EraseGui;
				class SequencerWindowGui;

				class EraseObserver
					: public moduru::observer::Observer
				{

				private:
					std::vector<std::string> typeNames{};
					std::vector<std::string> eraseNames{};
					std::weak_ptr<mpc::sampler::Sampler> sampler{ };
					std::weak_ptr<mpc::sampler::Program> program{};
					EraseGui* eraseGui{ nullptr };
					SequencerWindowGui* swGui{ nullptr };
					std::weak_ptr<mpc::lcdgui::Field> trackField{};
					std::weak_ptr<mpc::lcdgui::Label> trackNameLabel{};
					std::weak_ptr<mpc::lcdgui::Field> time0Field{};
					std::weak_ptr<mpc::lcdgui::Field> time1Field{};
					std::weak_ptr<mpc::lcdgui::Field> time2Field{};
					std::weak_ptr<mpc::lcdgui::Field> time3Field{};
					std::weak_ptr<mpc::lcdgui::Field> time4Field{};
					std::weak_ptr<mpc::lcdgui::Field> time5Field{};
					std::weak_ptr<mpc::lcdgui::Field> eraseField{};
					std::weak_ptr<mpc::lcdgui::Field> typeField{};
					std::weak_ptr<mpc::lcdgui::Field> notes0Field{};
					std::weak_ptr<mpc::lcdgui::Label> notes0Label{};
					std::weak_ptr<mpc::lcdgui::Field> notes1Field{};
					std::weak_ptr<mpc::lcdgui::Label> notes1Label{};
					mpc::sequencer::Sequence* sequence{ nullptr };
					int bus{ 0 };

				public:
					void update(moduru::observer::Observable* o, boost::any arg) override;

				private:
					void displayTrack();
					void displayTime();
					void displayErase();
					void displayType();
					void displayNotes();

				public:
					EraseObserver(mpc::Mpc* mpc);
					~EraseObserver();

				};

			}
		}
	}
}
