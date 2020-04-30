#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mpc {

	

	namespace file {
		namespace aps {

			class ApsGlobalParameters
			{

			private:
				bool padToInternalSound;
				bool padAssignMaster;
				bool stereoMixSourceDrum;
				bool indivFxSourceDrum;
				bool copyPgmMixToDrum;
				bool recordMixChanges;
				int masterLevel;
				int fxDrum;

			private:
				static std::vector<char> TEMPLATE;

			public:
				std::vector<char> saveBytes{};

			private:
				int readFxDrum(char b);

			public:
				int getFxDrum();
				bool isPadToIntSoundEnabled();
				bool isPadAssignMaster();
				bool isStereoMixSourceDrum();
				bool isIndivFxSourceDrum();
				bool isCopyPgmMixToDrumEnabled();
				bool isRecordMixChangesEnabled();
				int getMasterLevel();

			public:
				std::vector<char> getBytes();

			public:
				ApsGlobalParameters(std::vector<char> loadBytes);
				ApsGlobalParameters();

			};
		}
	}
}
