#include <audiomidi/DirectToDiskSettings.hpp>

using namespace mpc::audiomidi;

DirectToDiskSettings::DirectToDiskSettings(int lengthInFrames, std::string outputFolder, bool split, int sampleRate)
{
	this->lengthInFrames = lengthInFrames;
	this->outputFolder = outputFolder;
	this->split = split;
	this->sampleRate = sampleRate;
}
