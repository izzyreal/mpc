#pragma once
#include <controls/AbstractControls.hpp>

namespace mpc {

	class Mpc;

	namespace ui {
		namespace sequencer {
			class StepEditorGui;
			class EditSequenceGui;
			class SongGui;
			class BarCopyGui;
			class TrMoveGui;
			namespace window {
				class SequencerWindowGui;
			}
		}
	}

	namespace sequencer {
		class Sequence;
	}

	namespace controls {
		namespace sequencer {

			class AbstractSequencerControls
				: public AbstractControls

			{

			public:
				typedef AbstractControls super;

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
				
			protected:
				void checkAllTimesAndNotes(int notch);

			public:
				AbstractSequencerControls(Mpc* mpc);
				virtual ~AbstractSequencerControls();

			};

		}
	}
}
