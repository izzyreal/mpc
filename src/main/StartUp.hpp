#pragma once

//#include <lcdgui/LayeredScreen.hpp>
#include <ui/UserDefaults.hpp>

#include <thread>
#include <memory>

namespace mpc {
	
	class StartUp
	{

	private:
		std::thread startupThread;

	private:
		//std::thread spawn();
		static std::shared_ptr<ui::UserDefaults> userDefaults;

	public:
		static std::string home;
		static std::string resPath;
		static std::string tempPath;
		static std::string storesPath;
		static std::string logFilePath;

	public:
		void runStartUpRoutine();

	public:
		static std::weak_ptr<ui::UserDefaults> getUserDefaults();
		static void initUserDefaults();

		//void runBootSequence(std::weak_ptr<Gui> gui);

	public:
		StartUp();
		~StartUp();

	};

}
