#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {
	class Mpc;
	namespace sampler {
		class PgmSlider;
		class Sampler;
		class Program;
	}

	namespace lcdgui {
		class Field;
	}


	namespace ui {

		namespace sequencer {

			class AssignObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::sampler::PgmSlider* slider{ nullptr };
				std::weak_ptr<mpc::lcdgui::Field> assignNoteField{};
				std::weak_ptr<mpc::lcdgui::Field> parameterField{};
				std::weak_ptr<mpc::lcdgui::Field> highRangeField{};
				std::weak_ptr<mpc::lcdgui::Field> lowRangeField{};
				std::weak_ptr<mpc::lcdgui::Field> assignNvField{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				std::weak_ptr<mpc::sampler::Program> program{};

			private:
				void displayAssignNote();
				void displayParameter();
				void displayHighRange();
				void displayLowRange();
				void displayAssignNv();

			public:
				void update(moduru::observer::Observable* o, nonstd::any arg) override;

			public:
				AssignObserver(mpc::Mpc* mpc);
				~AssignObserver();
			};

		}
	}
}
