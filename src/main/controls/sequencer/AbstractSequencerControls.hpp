#pragma once
#include <controls/BaseControls.hpp>

namespace mpc::ui::sequencer {
	class StepEditorGui;
	class EditSequenceGui;
	class SongGui;
	class BarCopyGui;
	class TrMoveGui;
}

namespace mpc::ui::sequencer::window {
	class SequencerWindowGui;
}


namespace mpc::sequencer {
	class Sequence;
}

namespace mpc::controls::sequencer {

	class AbstractSequencerControls
		: public BaseControls

	{

	protected:
		std::weak_ptr<mpc::sequencer::Sequence> sequence{};
		std::weak_ptr<mpc::sequencer::Sequence> fromSeq{};
		std::weak_ptr<mpc::sequencer::Sequence> toSeq{};
		mpc::ui::sequencer::window::SequencerWindowGui* swGui{ nullptr };
		mpc::ui::sequencer::StepEditorGui* seGui{ nullptr };
		mpc::ui::sequencer::BarCopyGui* barCopyGui{ nullptr };
		mpc::ui::sequencer::SongGui* songGui{ nullptr };
		mpc::ui::sequencer::EditSequenceGui* editSequenceGui{ nullptr };
		mpc::ui::sequencer::TrMoveGui* trMoveGui{ nullptr };

	protected:
		void init() override;

	public:
		AbstractSequencerControls();

	};
}
