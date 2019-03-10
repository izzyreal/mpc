#pragma once

namespace mpc {

	namespace ui {
		namespace sequencer {
			class StepEditorGui;
		}
	}

	namespace command {

		class CopySelectedNotes
		{

		public:
			mpc::ui::sequencer::StepEditorGui* seGui{ nullptr };

		public:
			void execute();

			CopySelectedNotes(mpc::ui::sequencer::StepEditorGui* seGui);

		};
	}
}
