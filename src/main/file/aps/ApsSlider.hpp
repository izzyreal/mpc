#pragma once

#include <vector>

namespace mpc::sampler { class PgmSlider; }

namespace mpc::file::aps
{
	class ApsSlider
	{

	public:
		int note;
		int tuneLow;
		int tuneHigh;
		int decayLow;
		int decayHigh;
		int attackLow;
		int attackHigh;
		int filterLow;
		int filterHigh;
		int programChange;
		std::vector<char> saveBytes;

	private:
		static std::vector<char> PADDING;

	public:
		int getNote();
		int getTuneLow();
		int getTuneHigh();
		int getDecayLow();
		int getDecayHigh();
		int getAttackLow();
		int getAttackHigh();
		int getFilterLow();
		int getFilterHigh();
		int getProgramChange();
		std::vector<char> getBytes();

		ApsSlider(std::vector<char> loadBytes);
		ApsSlider(mpc::sampler::PgmSlider* slider);
	};
}
