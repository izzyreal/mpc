#pragma once
#include <engine/control/ControlLaw.hpp>
#include <vector>

namespace ctoot {
	namespace audio {
		namespace fader {

			class FaderLaw
				: public ctoot::control::ControlLaw
			{

			private:
				int resolution{ 1 };
				float halfdB{ 1 };
				float maxdB{ 1 };
				float attenuationCutoffFactor{ 1};
				std::vector<float> floatValues;
				int binarySearch(std::vector<float>& buf, float key, int min, int max);

			public:

                std::string getUnits();
				int intValue(float v);

            private:
				std::vector<float> createFloatValues();

			public:
				virtual float calculateFloatValue(int v);

			public:
				FaderLaw(int resolution, float halfdB, float maxdB, float attenuationCutoffFactor);
				FaderLaw(int resolution);
                virtual ~FaderLaw() {}
                
			};

		}
	}
}
