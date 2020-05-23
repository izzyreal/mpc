#pragma once
#include <controls/BaseControls.hpp>

namespace mpc::sampler
{
	class Sound;
	class Pad;
	class NoteParameters;
}

namespace mpc::ui::sampler::window
{
	class ZoomGui;
	class SamplerWindowGui;
	class EditSoundGui;
}

namespace mpc::ui::sampler
{
	class SoundGui;
}

namespace mpc::controls::sampler {

	class AbstractSamplerControls
		: public mpc::controls::BaseControls
	{

	protected:
		mpc::ui::sampler::window::ZoomGui* zoomGui{ nullptr };
		mpc::ui::sampler::SoundGui* soundGui{ nullptr };
		std::weak_ptr<mpc::sampler::Sound> sound{};
		mpc::ui::sampler::window::SamplerWindowGui* swGui{ nullptr };
		mpc::ui::sampler::window::EditSoundGui* editSoundGui{ nullptr };
		mpc::sampler::Pad* lastPad{ nullptr };
		mpc::sampler::NoteParameters* lastNp{ nullptr };

	protected:
		void init() override;

	protected:

	public:
		AbstractSamplerControls();

	};
}
