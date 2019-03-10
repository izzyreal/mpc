#pragma once
#include <observer/Observer.hpp>

#include <memory>

namespace mpc {

	class Mpc;

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

			class SamplerGui;

			namespace window {

				class MuteAssignObserver
					: public moduru::observer::Observer
				{

				private:
					mpc::Mpc* mpc{ nullptr };
					std::weak_ptr<mpc::sampler::Sampler> sampler{};
					mpc::ui::sampler::SamplerGui* samplerGui{ nullptr };
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
					void update(moduru::observer::Observable* o, std::any arg) override;

				public:
					MuteAssignObserver(mpc::Mpc* mpc);
					~MuteAssignObserver();

				};

			}
		}
	}
}
