#pragma once

#include <vector>
#include <memory>

namespace mpc {

	namespace sampler {
		class StereoMixerChannel;
		class IndivFxMixerChannel;
	}

	namespace file {
		namespace aps {

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
				mpc::sampler::StereoMixerChannel* getStereoMixerChannel(int note);
				mpc::sampler::IndivFxMixerChannel* getIndivFxMixerChannel(int note);
				int getFxPath(int note);
				int getLevel(int note);
				int getPanning(int note);
				int getIndividualLevel(int note);
				int getIndividualOutput(int note);
				int getSendLevel(int note);
				std::vector<char> getBytes();

			public:
				ApsMixer(std::vector<char> loadBytes);
				ApsMixer(std::vector<std::weak_ptr<mpc::sampler::StereoMixerChannel>> smc, std::vector<std::weak_ptr<mpc::sampler::IndivFxMixerChannel>> ifmc);
                
			};

		}
	}
}
