#pragma once

#include <observer/Observer.hpp>

#include <memory>
#include <vector>

namespace ctoot {
	namespace mpc {
		class MpcSoundPlayerChannel;
	}
}

namespace mpc {
	namespace sampler {
		class Sampler;
		class Program;
	}

	namespace sequencer {
		class Sequencer;
	}

	namespace disk {
		class AbstractDisk;
	}

	namespace lcdgui {
		class Field;
		class Label;
	}

	namespace ui {

		namespace disk {

			class DiskGui;

			class DiskObserver
				: public moduru::observer::Observer
			{

			private:

				DiskGui* diskGui{ nullptr };
				std::weak_ptr<mpc::disk::AbstractDisk> disk;
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
				std::weak_ptr<mpc::sampler::Sampler> sampler;
				ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };
				std::weak_ptr<mpc::sampler::Program> program;
				std::weak_ptr<mpc::lcdgui::Field> viewField;
				std::weak_ptr<mpc::lcdgui::Field> directoryField;
				std::weak_ptr<mpc::lcdgui::Field> fileField;
				std::weak_ptr<mpc::lcdgui::Field> typeField;
				std::weak_ptr<mpc::lcdgui::Field> deviceField;
				std::weak_ptr<mpc::lcdgui::Label> loadFileNameLabel;
				std::weak_ptr<mpc::lcdgui::Label> sizeLabel;
				std::weak_ptr<mpc::lcdgui::Label> freesndLabel;
				std::weak_ptr<mpc::lcdgui::Label> freeseqLabel;
				std::weak_ptr<mpc::lcdgui::Label> freeLabel;
				std::weak_ptr<mpc::lcdgui::Field> loadReplaceSoundField;
				std::weak_ptr<mpc::lcdgui::Field> assignToNoteField;
				std::string csn = "";
				std::weak_ptr<mpc::lcdgui::Field> loadIntoField;
				std::weak_ptr<mpc::lcdgui::Label> nameLabel;
				std::weak_ptr<mpc::lcdgui::Label> fileLabel;
				std::weak_ptr<mpc::lcdgui::Field> saveField;
				std::weak_ptr<mpc::lcdgui::Field> replaceSameSoundsField;
				std::weak_ptr<mpc::lcdgui::Field> fileTypeField;
				std::weak_ptr<mpc::lcdgui::Field> saveAsField;

			public:
				DiskObserver();
			};
		}
	}
}
