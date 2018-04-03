#pragma once

#include <vector>
#include <memory>

namespace ctoot {
	namespace mpc {
		class MpcStereoMixerChannel;
		class MpcIndivFxMixerChannel;
	}
}

namespace mpc {

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
				ctoot::mpc::MpcStereoMixerChannel* getStereoMixerChannel(int note);
				ctoot::mpc::MpcIndivFxMixerChannel* getIndivFxMixerChannel(int note);
				int getFxPath(int note);
				int getLevel(int note);
				int getPanning(int note);
				int getIndividualLevel(int note);
				int getIndividualOutput(int note);
				int getSendLevel(int note);
				std::vector<char> getBytes();

			public:
				ApsMixer(std::vector<char> loadBytes);
				ApsMixer(std::vector<std::weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> smc, std::vector<std::weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> ifmc);
                
			};

		}
	}
}
