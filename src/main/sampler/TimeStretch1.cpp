#include "TimeStretch1.hpp"

#include <Logger.hpp>

using namespace mpc::sampler;

TimeStretch::TimeStretch(std::vector<float> sampleData, float ratio, int sampleRate, int adjust)
{
	int oldLength = sampleData.size();

	int desiredNewLength = oldLength * ratio;

	MLOG("desired new length: " + std::to_string(desiredNewLength));

	cycleLength = 1500;
	const int fadeLength = 250;
	
	const int cycleLengthWithoutFades = cycleLength - fadeLength;

	//int jump = 750;
	desiredNewLength -= fadeLength;
	const int numberOfJumps = desiredNewLength / cycleLengthWithoutFades;
	//const int jump = numberOfJumps + 1

	//jump * (numberOfJumps + 1) = oldLength
	const int jump = oldLength / (numberOfJumps + 1);

	/*
	int cycleLengthWithoutFades = cycleLength - fadeLength;
	int numberOfJumps = floor(oldLength / jump) - 1;
	int newLength = numberOfJumps * cycleLengthWithoutFades;
	newLength += fadeLength;

	MLOG("old length: " + std::to_string(oldLength));
	MLOG("estimated new length: " + std::to_string(newLength));
	*/

	int currentPos = 0;
	while (currentPos + cycleLength < sampleData.size()) {
		auto segment = seg(&sampleData, cycleLength, currentPos);
		fade(fadeLength, &segment);
		for (int i = 0; i < fadeLength; i++) {
			float f1 = segment[i];
			if (currentPos != 0) {
				float f2 = processedData[processedData.size() - (fadeLength - i)];
				processedData[processedData.size() - (fadeLength - i)] = f1 + f2;
			}
			else {
				processedData.push_back(f1);
			}
		}
		for (int i = fadeLength; i < segment.size(); i++) {
			processedData.push_back(segment[i]);
		}
		currentPos += jump;
	}
	MLOG("actual new length: " + std::to_string(processedData.size()));
}

std::vector<float> TimeStretch::getProcessedData()
{
    return processedData;
}

std::vector<float> TimeStretch::seg(std::vector<float>* src, int length, int offset) {
    std::vector<float> res;
	for (int i = 0; i < length; i++)
		res.push_back((*src)[i + offset]);
	return res;
}

void TimeStretch::fade(int length, std::vector<float>* fa)
{
	float ampCoeff = 0.0f;
	float inc = 1.0f / ((float)(length));
	int fadeOutOffset = fa->size() - length;
	for (int i = 0; i < length; i++) {
		(*fa)[i] *= ampCoeff;
		ampCoeff += inc;
	}
	ampCoeff = 1.0f;
	for (int i = fadeOutOffset; i < fa->size(); i++) {
		(*fa)[i] *= ampCoeff;
		ampCoeff -= inc;
	}
}

float TimeStretch::value(int length, int index)
{
    //return 0.5f * (1.0f - static_cast< float >(cos(TWO_PI * index / (length - 1.0f))));
    return 0;
}
