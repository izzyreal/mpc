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

			namespace window {
				class DiskWindowGui;
			}

			class DiskObserver
				: public moduru::observer::Observer
			{

			private:
				const std::vector<std::string> views{ "All Files", ".SND", ".PGM", ".APS", ".MID", ".ALL", ".WAV", ".SEQ", ".SET" };
				const std::vector<std::string> types{ "Save All Sequences & Songs", "Save a Sequence", "Save All Program and Sounds", "Save a Program & Sounds", "Save a Sound", "Copy Operating System" };
				const std::vector<std::string> pgmSaveNames{ "PROGRAM ONLY", "WITH SOUNDS", "WITH .WAV" };
				const std::vector<std::string> apsSaveNames{ "APS ONLY", "WITH SOUNDS", "WITH .WAV" };

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
				std::string csn{ "" };
				std::weak_ptr<mpc::lcdgui::Field> loadIntoField;
				mpc::ui::disk::window::DiskWindowGui* diskWindowGui;
				std::weak_ptr<mpc::lcdgui::Label> nameLabel;
				std::weak_ptr<mpc::lcdgui::Label> fileLabel;
				std::weak_ptr<mpc::lcdgui::Field> saveField;
				std::weak_ptr<mpc::lcdgui::Field> replaceSameSoundsField;
				std::weak_ptr<mpc::lcdgui::Field> fileTypeField;
				std::weak_ptr<mpc::lcdgui::Field> saveAsField;

			private:
				void displaySaveAs();
				void displayFileType();
				void displaySave();
				void displayReplaceSameSounds();
				void displayFree();
				void displayDevice();
				void displayFreeSnd();
				void displayAssignToNote();
				void displaySize();
				void displayLoadInto();
				void displayFile();

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

			private:
				void displayType();
				void displayLoadReplaceSound();
				void displayView();
				void displayDirectory();
				std::string padExtensionRight(std::string s);

			public:
				DiskObserver();
				~DiskObserver();
			};

		}
	}
}
