#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	

	namespace sampler {
		class Sampler;
		class Program;
		class NoteParameters;
	}

	namespace lcdgui {
		class Field;
	}


	namespace ui {
		namespace sampler {

			namespace window {

				class MuteAssignObserver
					: public moduru::observer::Observer
				{

				private:
					
					std::weak_ptr<mpc::sampler::Sampler> sampler{};
					std::weak_ptr<mpc::lcdgui::Field> noteField{};
					std::weak_ptr<lcdgui::Field> note0Field{};
					std::weak_ptr<lcdgui::Field> note1Field{};
					std::weak_ptr<mpc::sampler::Program> program{};
					mpc::sampler::NoteParameters* np{ nullptr };

				private:
					void displayNote();
					void displayNote0();
					void displayNote1();

				public:
					void update(moduru::observer::Observable* o, nonstd::any arg) override;

				public:
					MuteAssignObserver();
					~MuteAssignObserver();

				};

			}
		}
	}
}
