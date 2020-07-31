#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::disk
{
	class DiskController;
}

namespace mpc::lcdgui::screens
{
	class VmpcDiskScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

		VmpcDiskScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		const std::vector<std::string> types{ "OFF", "STD", "RAW" };
		int scsi = 0;
		int scsiPlaceHolder = 0;
		std::vector<int> accessTypes = std::vector<int>(7);
		std::vector<int> stores{ 0, -1, -1, -1, -1, -1, -1 };
		std::vector<int> accessTypesPlaceHolder = std::vector<int>(7);
		std::vector<int> storesPlaceHolder{ 0, -1, -1, -1, -1, -1, -1 };

		void displayScsi();
		void displayAccessType();
		void displayRoot();

	private:
		static const int OFF = 0;
		static const int STD = 1;
		static const int RAW = 2;

		void setScsi(int i);
		void setStore(int i, int store);
		int getStore(int i);
		bool isEnabled(int i);
		bool isRaw(int i);
		int getAccessType(int i);
		void setAccessType(int i, int type);
		void saveSettings();
		void restoreSettings();

		friend class mpc::disk::DiskController;
	};
}
