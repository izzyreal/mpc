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

	class Mpc;

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

		namespace sampler {
			class SamplerGui;
			class SoundGui;
		}

		namespace disk {

			class DiskGui;

			namespace window {
				class DiskWindowGui;
			}

			class DiskObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				std::vector<std::string> views{};
				std::vector<std::string> types{};
				std::vector<std::string> pgmSaveNames{};
				std::vector<std::string> apsSaveNames{};

				DiskGui* diskGui{ nullptr };
				mpc::ui::sampler::SoundGui* soundGui{ nullptr };
				std::weak_ptr<mpc::disk::AbstractDisk> disk{};
				std::weak_ptr<mpc::sequencer::Sequencer> sequencer{};
				std::weak_ptr<mpc::sampler::Sampler> sampler{};
				ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel{ nullptr };
				std::weak_ptr<mpc::sampler::Program> program{};
				mpc::ui::sampler::SamplerGui* samplerGui{ nullptr };
				std::weak_ptr<mpc::lcdgui::Field> viewField{};
				std::weak_ptr<mpc::lcdgui::Field> directoryField{};
				std::weak_ptr<mpc::lcdgui::Field> fileField{};
				std::weak_ptr<mpc::lcdgui::Field> typeField{};
				std::weak_ptr<mpc::lcdgui::Field> deviceField{};
				std::weak_ptr<mpc::lcdgui::Label> loadFileNameLabel{};
				std::weak_ptr<mpc::lcdgui::Label> sizeLabel{};
				std::weak_ptr<mpc::lcdgui::Label> freesndLabel{};
				std::weak_ptr<mpc::lcdgui::Label> freeseqLabel{};
				std::weak_ptr<mpc::lcdgui::Label> freeLabel{};
				std::weak_ptr<mpc::lcdgui::Field> loadReplaceSoundField{};
				std::weak_ptr<mpc::lcdgui::Field> assignToNoteField{};
				std::string csn{ "" };
				std::weak_ptr<mpc::lcdgui::Field> loadIntoField{};
				mpc::ui::disk::window::DiskWindowGui* diskWindowGui{};
				std::weak_ptr<mpc::lcdgui::Label> nameLabel{};
				std::weak_ptr<mpc::lcdgui::Label> fileLabel{};
				std::weak_ptr<mpc::lcdgui::Field> saveField{};
				std::weak_ptr<mpc::lcdgui::Field> replaceSameSoundsField{};
				std::weak_ptr<mpc::lcdgui::Field> fileTypeField{};
				std::weak_ptr<mpc::lcdgui::Field> saveAsField{};

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
				DiskObserver(mpc::Mpc* mpc);
				~DiskObserver();
			};

		}
	}
}
