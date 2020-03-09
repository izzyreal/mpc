#pragma once
#include <observer/Observer.hpp>

#include <memory>
#include <vector>

namespace mpc {

	class Mpc;

	namespace lcdgui {
		class Field;
	}

	namespace ui {

		namespace vmpc {

			class DirectToDiskRecorderGui;

			class DirectToDiskRecorderObserver
				: public moduru::observer::Observer
			{

			private:
				mpc::Mpc* mpc;
				std::vector<std::string> recordNames{};
				std::weak_ptr<mpc::lcdgui::Field> recordField{};
				std::weak_ptr<mpc::lcdgui::Field> sqField{};
				std::weak_ptr<mpc::lcdgui::Field> songField{};
				std::weak_ptr<mpc::lcdgui::Field> time0Field{};
				std::weak_ptr<mpc::lcdgui::Field> time1Field{};
				std::weak_ptr<mpc::lcdgui::Field> time2Field{};
				std::weak_ptr<mpc::lcdgui::Field> time3Field{};
				std::weak_ptr<mpc::lcdgui::Field> time4Field{};
				std::weak_ptr<mpc::lcdgui::Field> time5Field{};
				std::weak_ptr<mpc::lcdgui::Field> outputFolderField{};
				std::weak_ptr<mpc::lcdgui::Field> offlineField{};
				std::weak_ptr<mpc::lcdgui::Field> splitLRField{};
				std::weak_ptr<mpc::lcdgui::Field> rateField{};
				DirectToDiskRecorderGui* d2dRecorderGui{};

			private:
				void displayRate();
				void displaySong();
				void displayOffline();
				void displaySplitLR();
				void displayOutputFolder();
				void displayRecord();
				void displaySq();
				void displayTime();

			public:
				void update(moduru::observer::Observable* o, nonstd::any a) override;

				DirectToDiskRecorderObserver(mpc::Mpc* mpc);
				~DirectToDiskRecorderObserver();
			};

		}
	}
}
