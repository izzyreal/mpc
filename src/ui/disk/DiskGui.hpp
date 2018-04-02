#pragma once

#include <observer/Observable.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mpc {

	class Mpc;

	namespace disk {
		class MpcFile;
	}

	namespace sequencer {
		class Sequence;
	}

	namespace ui {

		namespace disk {

			class DiskGui
				: public moduru::observer::Observable
			{

			private:
				mpc::Mpc* mpc;
				std::vector<std::shared_ptr<mpc::sequencer::Sequence>>* sequencesFromAllFile{};
				int fileLoad{ 0 };
				int deleteIndex { 0 };
				int view{ 0 };
				int type{ 0 };
				bool loadReplaceSound{ false };
				bool clearProgramWhenLoading{ false };
				bool waitingForUser{ false };
				bool skipAll{ false };
				std::string cannotFindFileName{ "" };
				bool saveReplaceSameSounds{ false };
				int save{ 0 };
				int fileTypeSaveSound{ 0 };
				int saveSequenceAs{ 1 };
				bool dontAssignSoundWhenLoading_{ false };

			public:
				void setType(int i);
				int getType();
				mpc::disk::MpcFile* getSelectedFile();
				std::string getSelectedFileName();
				void setDelete(int i);
				int getDelete();
				bool isSelectedFileDirectory();
				int getFileSize(int i);
				int getView();
				void setView(int i);
				bool getLoadReplaceSound();
				void setLoadReplaceSound(bool b);
				int getFileLoad();
				void setSelectedFileNumberLimited(int i);
				void setFileLoad(int i);
				void openPopup(std::string soundFileName, std::string extension);
				void removePopup();
				bool getClearProgramWhenLoading();
				void setClearProgramWhenLoading(bool b);
				void setWaitingForUser(bool b);
				bool isWaitingForUser();
				void setSkipAll(bool b);
				bool getSkipAll();
				void setCannotFindFileName(std::string s);
				std::string* getCannotFindFileName();
				int getPgmSave();
				bool getSaveReplaceSameSounds();
				void setSaveReplaceSameSounds(bool b);
				void setSave(int i);
				int getFileTypeSaveSound();
				void setFileTypeSaveSound(int i);
				int getSaveSequenceAs();
				void setSaveSequenceAs(int i);
				bool dontAssignSoundWhenLoading();
				void setDontAssignSoundWhenLoading(bool b);
				void setSequencesFromAllFile(std::vector<std::shared_ptr<mpc::sequencer::Sequence>>* sequences);
				std::vector<std::shared_ptr<mpc::sequencer::Sequence>>* getSequencesFromAllFile();

				DiskGui(mpc::Mpc* mpc);
				~DiskGui();

			};

		}
	}
}
