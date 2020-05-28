#pragma once

#include <vector>
#include <string>

namespace mpc::ui {
	class UserDefaults;
}

namespace mpc::file::all {

	class Defaults
	{

	public:
		static const int DEF_SEQ_NAME_OFFSET{ 0 };
		static const int UNKNOWN1_OFFSET{ 16 };
		static std::vector<char> UNKNOWN1;

	public:
		static const int TEMPO_BYTE1_OFFSET{ 22 };
		static const int TEMPO_BYTE2_OFFSET{ 23 };
		static const int TIMESIG_NUM_OFFSET{ 24 };
		static const int TIMESIG_DEN_OFFSET{ 25 };
		static const int BAR_COUNT_BYTE1_OFFSET{ 26 };
		static const int BAR_COUNT_BYTE2_OFFSET{ 27 };
		static const int LAST_TICK_BYTE1_OFFSET{ 28 };
		static const int LAST_TICK_BYTE2_OFFSET{ 29 };
		static const int LAST_TICK_BYTE3_OFFSET{ 30 };
		static const int UNKNOWN32_BIT_INT_OFFSET{ 32 };
		static const int UNKNOWN2_OFFSET{ 48 };
		static std::vector<char> UNKNOWN2;

	public:
		static const int DEV_NAMES_OFFSET{ 120 };
		static const int DEV_NAMES_LENGTH{ 264 };
		static const int TR_NAMES_OFFSET{ 384 };
		static const int TR_NAMES_LENGTH{ 1024 };
		static const int DEVICES_OFFSET{ 1408 };
		static const int DEVICES_LENGTH{ 64 };
		static const int BUSSES_OFFSET{ 1472 };
		static const int BUSSES_LENGTH{ 64 };
		static const int PGMS_OFFSET{ 1536 };
		static const int PGMS_LENGTH{ 64 };
		static const int TR_VELOS_OFFSET{ 1600 };
		static const int TR_VELOS_LENGTH{ 64 };
		static const int TR_STATUS_OFFSET{ 1664 };
		static const int TR_STATUS_LENGTH{ 64 };
		std::string defaultSeqName = "";
		int tempo{};
		int timeSigNum{};
		int timeSigDen{};
		int barCount{};

		std::vector<std::string> devNames = std::vector<std::string>(33);
		std::vector<std::string> trackNames = std::vector<std::string>(64);
		std::vector<int> devices = std::vector<int>(64);
		std::vector<int> busses = std::vector<int>(64);
		std::vector<int> pgms = std::vector<int>(64);
		std::vector<int> trVelos = std::vector<int>(64);
		std::vector<int> status = std::vector<int>(64);
		std::vector<char> saveBytes{};

	private:
		void parseNames(std::vector<char> loadBytes);

	public:
		std::string getDefaultSeqName();
		int getTempo();
		int getTimeSigNum();
		int getTimeSigDen();
		int getBarCount();
		std::vector<std::string> getDefaultDevNames();
		std::vector<std::string> getDefaultTrackNames();
		std::vector<int> getDevices();
		std::vector<int> getBusses();
		std::vector<int> getPgms();
		std::vector<int> getTrVelos();

	private:
		void setTrackSettings(mpc::ui::UserDefaults& ud);
		void setLastTick(mpc::ui::UserDefaults& ud);
		void setBarCount(mpc::ui::UserDefaults& ud);
		void setTimeSig(mpc::ui::UserDefaults& ud);

	public:
		void setNames(mpc::ui::UserDefaults& ud);
		void setTempo(mpc::ui::UserDefaults& ud);

	public:
		std::vector<char> getBytes();

		Defaults(std::vector<char> loadBytes);
		Defaults(mpc::ui::UserDefaults& ud);
	};
}
