#pragma once

#include <vector>

namespace mpc {
	namespace sampler {

		class TimeStretch
		{

		private:
			static constexpr float TWO_PI{ 6.2831855f };
			int cycleLength = 1000;
			std::vector<float> processedData;

		public:
			virtual std::vector<float> getProcessedData();

		private:
			void fade(int length, std::vector<float>* fa);
			std::vector<float> seg(std::vector<float>* src, int length, int offset);

		private:
			virtual float value(int length, int index);


		public:
			TimeStretch(std::vector<float> sampleData, float ratio, int sampleRate, int adjust);

		};

	}
}
