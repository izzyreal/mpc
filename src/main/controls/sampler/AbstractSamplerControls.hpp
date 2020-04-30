#pragma once
#include <controls/BaseControls.hpp>

namespace mpc {

	namespace sampler {
		class Sound;
		class Pad;
		class NoteParameters;
	}

	namespace ui {
		namespace sampler {
			namespace window {
				class ZoomGui;
				class SamplerWindowGui;
				class EditSoundGui;
			}
			class SoundGui;
		}
	}

	namespace controls {
		namespace sampler {

			class AbstractSamplerControls
				: public mpc::controls::BaseControls
			{

			public:
				typedef mpc::controls::BaseControls super;

			protected:
				mpc::ui::sampler::window::ZoomGui* zoomGui{ nullptr };
				mpc::ui::sampler::SoundGui* soundGui{ nullptr };
				std::weak_ptr<mpc::sampler::Sound> sound{};
				mpc::ui::sampler::window::SamplerWindowGui* swGui{ nullptr };
				mpc::ui::sampler::window::EditSoundGui* editSoundGui{ nullptr };
				mpc::sampler::Pad* lastPad{ nullptr };
				mpc::sampler::NoteParameters* lastNp{ nullptr };

				bool splittable{ false };
				std::vector<int> splitInc{};
				
			protected:
				void init() override;
				
			protected:
				int getSoundIncrement(int notch_inc);
				void splitLeft();
				void splitRight();

			public:
				AbstractSamplerControls();
				virtual ~AbstractSamplerControls();

			};

		}
	}
}
