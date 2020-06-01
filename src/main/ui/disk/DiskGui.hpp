#pragma once

#include <observer/Observable.hpp>

#include <memory>
#include <string>
#include <vector>

namespace mpc::disk {
	class MpcFile;
}

namespace mpc::sequencer {
	class Sequence;
}

namespace mpc::ui::disk {

	class DiskGui
		: public moduru::observer::Observable
	{

	private:
		int deleteIndex{ 0 };
		bool waitingForUser{ false };
		bool skipAll{ false };
		std::string cannotFindFileName = "";
		bool saveReplaceSameSounds{ false };
		bool dontAssignSoundWhenLoading_{ false };

	public:
		void setDelete(int i);
		int getDelete();
		int getFileSize(int i);
		void openPopup(std::string soundFileName, std::string extension);
		void removePopup();
		void setWaitingForUser(bool b);
		bool isWaitingForUser();
		void setSkipAll(bool b);
		bool getSkipAll();
		void setCannotFindFileName(std::string s);
		std::string* getCannotFindFileName();
		bool getSaveReplaceSameSounds();
		void setSaveReplaceSameSounds(bool b);
		bool dontAssignSoundWhenLoading();
		void setDontAssignSoundWhenLoading(bool b);

		DiskGui();

	};
}
