#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc {

	namespace ui {
		
		namespace sampler {

			class SoundGui
				: public moduru::observer::Observable
			{

			private:
				std::string previousScreenName{""};
				int convert{};
				int newFs{};
				int quality{};
				int newBit{};
				std::string newName{ "" };
				std::string newLName{ "" };
				std::string newRName{ "" };
				int rSource{};
				std::string newStName{ "" };
				int view{};
				int playX{};
				bool endSelected{};
				int numberOfZones{ 16 };
				int previousNumberOfZones{ 0 };
				std::vector<std::vector<int>> zones{};
				int zone{};
				int totalLength{};

			public:
				bool isEndSelected();
				void setEndSelected(bool b);
				std::string getPreviousScreenName();
				void setPreviousScreenName(std::string s);
				void setConvert(int i);
				int getConvert();
				void setNewFs(int i);
				int getNewFs();
				void setQuality(int i);
				int getQuality();
				void setNewBit(int i);
				int getNewBit();
				void setNewName(std::string s);
				std::string getNewName();
				void setNewLName(std::string s);
				std::string getNewLName();
				void setNewRName(std::string s);
				std::string getNewRName();
				int getRSource();
				std::string getNewStName();
				void setRSource(int i, int soundCount);
				void setNewStName(std::string s);
				void setPlayX(int i);
				int getPlayX();
				void setView(int i);
				int getView();
				void initZones(int length);
				void setZoneStart(int zoneIndex, int start);
				int getZoneStart(int zoneIndex);
				void setZoneEnd(int zoneIndex, int end);
				int getZoneEnd(int zoneIndex);
				void setZone(int i);
				int getZoneNumber();
				void setNumberOfZones(int i);
				int getNumberOfZones();
				void setPreviousNumberOfZones(int i);
				int getPreviousNumberOfzones();

			public:
				SoundGui();

			};

		}
	}
}
