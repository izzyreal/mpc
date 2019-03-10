#pragma once
#include <observer/Observable.hpp>

#include <vector>

namespace mpc {
	namespace ui {
		namespace vmpc {

			class DeviceGui
				: public moduru::observer::Observable
			{

			private:
				int scsi{ 0 };
				std::vector<int> accessTypes{};
				std::vector<int> stores{};
				std::vector<int> accessTypesPlaceHolder{};
				std::vector<int> storesPlaceHolder{};
				int scsiPlaceHolder{ 0 };

			public:
				static const int OFF{ 0 };
				static const int STD{ 1 };
				static const int RAW{ 2 };

			private:
				bool initialized{ false };

			public:
				void setScsi(int i);
				void setStore(int i, int store);
				int getStore(int i);
				int getScsi();

			private:
				void notifyAccessType();

			public:
				bool isEnabled(int i);
				bool isRaw(int i);
				int getAccessType(int i);
				void setAccessType(int i, int type);
				void saveSettings();
				void restoreSettings();

				DeviceGui();
			};

		}
	}
}
