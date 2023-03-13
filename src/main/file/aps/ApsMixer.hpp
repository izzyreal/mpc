#pragma once

#include <vector>
#include <memory>

namespace ctoot::mpc
{
	class MpcStereoMixerChannel;
	class MpcIndivFxMixerChannel;
}

namespace mpc::file::aps
{
	class ApsMixer
	{

	public:
		std::vector<int> fxPaths = std::vector<int>(64);
		std::vector<int> levels = std::vector<int>(64);
		std::vector<int> pannings = std::vector<int>(64);
		std::vector<int> iLevels = std::vector<int>(64);
		std::vector<int> iOutputs = std::vector<int>(64);
		std::vector<int> sendLevels = std::vector<int>(64);
		std::vector<char> saveBytes = std::vector<char>(384);

	public:
		ctoot::mpc::MpcStereoMixerChannel getStereoMixerChannel(int noteIndex);
		ctoot::mpc::MpcIndivFxMixerChannel getIndivFxMixerChannel(int noteIndex);
		int getFxPath(int noteIndex);
		int getLevel(int noteIndex);
		int getPanning(int noteIndex);
		int getIndividualLevel(int noteIndex);
		int getIndividualOutput(int noteIndex);
		int getSendLevel(int noteIndex);
		std::vector<char> getBytes();

	public:
		ApsMixer(const std::vector<char>& loadBytes);
		ApsMixer(std::vector<std::shared_ptr<ctoot::mpc::MpcStereoMixerChannel>>& smc, std::vector<std::shared_ptr<ctoot::mpc::MpcIndivFxMixerChannel>>& ifmc);

	};
}
