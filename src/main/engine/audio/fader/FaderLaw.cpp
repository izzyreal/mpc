#include <engine/audio/fader/FaderLaw.hpp>

#include <algorithm>

using namespace ctoot::audio::fader;
using namespace std;

FaderLaw::FaderLaw(int resolution, float halfdB, float maxdB, float attenuationCutoffFactor) 
{
	this->resolution = resolution;
	this->halfdB = halfdB;
	this->maxdB = maxdB;
	this->attenuationCutoffFactor = attenuationCutoffFactor;
}

FaderLaw::FaderLaw(int resolution) 
{
	this->resolution = resolution;
}

string FaderLaw::getUnits()
{
	return "dB";
}

int FaderLaw::intValue(float v)
{
	if (floatValues.size() == 0) {
		floatValues = createFloatValues();
	}

	if (v <= floatValues[0]) return 0;

	if (v >= floatValues[resolution - 1]) return resolution - 1;

	auto ret = binarySearch(floatValues, v, 0, 100);
	if (ret >= 0) return ret;

	return -(++ret);
}

int FaderLaw::binarySearch(vector<float>& buf, float key, int min, int max)
{
	float mid;
	float fMax = static_cast<float>(max);
	float fMin = static_cast<float>(min);

	while (fMax >= fMin) {
		mid = (fMin + fMax) * 0.5;

		if (buf[static_cast<int>(mid)] < key)
			fMin = mid + 1;
		else if (buf[static_cast<int>(mid)] > key)
			fMax = mid - 1;

		else
			return static_cast<int>(mid);

	}
	return std::min(min, max);
}

vector<float> FaderLaw::createFloatValues()
{
	auto vals = vector<float>(resolution);
    for (auto i = 0; i < resolution; i++) {
        vals[i] = calculateFloatValue(i);
    }
    return vals;
}

float FaderLaw::calculateFloatValue(int v)
{
	auto val = static_cast<float>(v) / (resolution - 1);
	val = maxdB + (2 * halfdB * (1 - val));
	auto linZerodB = maxdB + (2 * halfdB);
	auto cutoff = static_cast<int>(attenuationCutoffFactor * (resolution - 1));
	if (v < cutoff) {
		auto attenFactor = static_cast<float>((cutoff - v)) / cutoff;
		attenFactor *= attenFactor;
		val -= (100.0f + linZerodB) * attenFactor;
	}
	return val;
}