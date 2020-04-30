#pragma once

#include <observer/Observable.hpp>
#include <thirdp/bcmath/bcmath_stl.h>
#include <sequencer/TimeSignature.hpp>

namespace mpc::ui {

	class UserDefaults final
		: public moduru::observer::Observable
	{

	private:
		std::vector<int> padNotes{};
		int diskDevice{ 0 };
		int autoLoadType{ 0 };
		BCMath tempo{ 0 };
		bool loop{ false };
		int bus{ 0 };
		int deviceNumber{ 0 };
		int pgm{ 0 };
		int velo{ 0 };
		bool recordingModeMulti{ false };
		std::string sequenceName{ "" };
		std::vector<std::string> trackNames{};
		int lastBar{ 0 };
		mpc::sequencer::TimeSignature timeSig;
		std::vector<std::string> deviceNames{};

	public:
		std::string getDeviceName(int i);

	public:
		void setDiskDevice(int i);
		void setAutoLoadType(int i);

	public:
		void setTempo(BCMath i);
		void setLoop(bool b);
		BCMath getTempo();
		int getDiskDevice();
		int getAutoLoadType();
		bool isRecordingModeMulti();
		void setBus(int i);
		void setDeviceNumber(int i);
		void setRecordingModeMulti(bool b);
		bool isLoopEnabled();
		int getBus();
		int getDeviceNumber();
		int getPgm();
		int getVeloRatio();
		void setLastBar(int i);
		void setPgm(int i);
		void setVelo(int i);
		std::string getSequenceName();
		std::string getTrackName(int i);
		int getLastBarIndex();
		mpc::sequencer::TimeSignature& getTimeSig();
		std::vector<int> getPadNotes();
		void setPadNotes(std::vector<int> ia);
		int8_t getTrackStatus();
		void setDeviceName(int i, std::string s);
		void setSequenceName(std::string name);
		void setTimeSig(int num, int den);
		void setTrackName(int i, std::string s);

	public:
		UserDefaults();
		~UserDefaults();

	};
}
