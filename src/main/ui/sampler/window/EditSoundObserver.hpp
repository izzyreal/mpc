#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>
#include <string>

namespace mpc::lcdgui {
	class Field;
	class Label;
}


namespace mpc::sampler {
	class Sampler;
}

namespace mpc::ui::sampler::window {
	class EditSoundGui;
}

namespace mpc::ui::sampler::window {

	class EditSoundObserver
		: public moduru::observer::Observer
	{

	private:
		EditSoundGui* editSoundGui{ nullptr };
		std::weak_ptr<mpc::sampler::Sampler> sampler{ };
		std::weak_ptr<mpc::lcdgui::Field> editField{};
		std::weak_ptr<mpc::lcdgui::Field> variable0Field{};
		std::weak_ptr<mpc::lcdgui::Label> variable0Label{};
		std::weak_ptr<mpc::lcdgui::Field> ratioField{};
		std::weak_ptr<mpc::lcdgui::Label> ratioLabel{};
		std::weak_ptr<mpc::lcdgui::Field> presetField{};
		std::weak_ptr<mpc::lcdgui::Label> presetLabel{};
		std::weak_ptr<mpc::lcdgui::Field> adjustField{};
		std::weak_ptr<mpc::lcdgui::Label> adjustLabel{};
		std::weak_ptr<mpc::lcdgui::Label> endMarginLabel{};
		std::weak_ptr<mpc::lcdgui::Field> endMarginField{};
		std::weak_ptr<mpc::lcdgui::Label> createNewProgramLabel{};
		std::weak_ptr<mpc::lcdgui::Field> createNewProgramField{};

		std::vector<std::string> editNames{ "DISCARD", "LOOP FROM ST TO END", u8"SECTION \u00C4 NEW SOUND", u8"INSERT SOUND \u00C4 SECTION START", "DELETE SECTION", "SILENCE SECTION", "REVERSE SECTION", "TIME STRETCH", "SLICE SOUND" };
		std::vector<std::string> timeStretchPresetNames{};

	private:
		void displayEdit();
		void displayCreateNewProgram();
		void displayEndMargin();
		void displayVariable();

	public:
		void update(moduru::observer::Observable* o, nonstd::any arg) override;

		EditSoundObserver();
		~EditSoundObserver();

	};
}
