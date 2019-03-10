#pragma once

namespace mpc {

	namespace ui {
		namespace sequencer {
			class StepEditorGui;
		}
	}

	namespace sequencer {
		class Track;
	}

	namespace command {

		class RemoveEvents
		{

		private:
			mpc::ui::sequencer::StepEditorGui* seqGui{};
			mpc::sequencer::Track* track{};

		public:
			void execute();

			RemoveEvents(mpc::ui::sequencer::StepEditorGui* seqGui, mpc::sequencer::Track* track);
		};

	}
}
