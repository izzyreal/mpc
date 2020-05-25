#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc::ui::sampler
{
	class SoundGui
		: public moduru::observer::Observable
	{

	private:
		int view = 0;
		int playX = 0;
		int numberOfZones = 16;
		int previousNumberOfZones = 0;
		std::vector<std::vector<int>> zones;
		int zone;
		int totalLength;

	public:
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

	};
}
