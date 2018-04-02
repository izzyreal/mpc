#pragma once

namespace mpc {

	namespace sequencer {
		class Track;
		class Sequencer;
	}

	namespace ui {
		namespace sequencer {
			class StepEditorGui;
		}
	}

	namespace command {

		class InsertEvent
		{

		private:
			mpc::ui::sequencer::StepEditorGui* seGui{};
			mpc::sequencer::Track* track{};
			mpc::sequencer::Sequencer* sequencer{};

		public:
			virtual void execute();

			InsertEvent(mpc::ui::sequencer::StepEditorGui* seGui, mpc::sequencer::Track* track, mpc::sequencer::Sequencer* sequencer);
		};
	}
}
