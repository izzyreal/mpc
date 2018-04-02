#pragma once

#include <string>

namespace mpc {

	namespace ui {
		namespace sequencer {
			class StepEditorGui;
		}
	}

	namespace command {

		class CopySelectedNote
		{

		private:
			std::string focus{ "" };
			mpc::ui::sequencer::StepEditorGui* seGui{};

		public:
			void execute();

			CopySelectedNote(std::string focus, mpc::ui::sequencer::StepEditorGui* seGui);
		};

	}
}
